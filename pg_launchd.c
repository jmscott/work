/*
 *  Synopsis:
 *	MAC/OSX launchd wrapper around postgres for fast halt with SIGINT
 *  Description:
 *	pg_launchd only exists because launchd on MAC/OSX can not send a
 *	SIGINT to do a fast and safe shutdown.
 *	
 *	pg_launchd is a wrapper program that invokes the postgres master
 *	with a fast shutdown using SIGINT after receiving a SIGTERM from
 *	launchd.  The time to pause before Also, the pid file is forcibly
 *	removed.
 *  Usage:
 *	pg_launchd							\
 *		--SIGINT-pause 5					\
 *		--pid-path /usr/local/pgsql/data/postmaster.pid		\
 *		/usr/local/pgsql/bin/postgres				\
 *			-D /usr/local/pgsql/data
 *
 *	#  contents of /Library/LaunchDaemons/org.postgresql.plist
 *	<key>ProgramArguments</key>
 *	<array>
 *		<string>/Users/postgres/sbin/pg_launchd</string>
 *		<string>--SIGINT-pause</string>
 *		<string>30</string>
 *		<string>--pid-path</string>
 *		<string>/usr/local/pgsql/data/postmaster.pid</string>
 *		<string>/usr/local/pgsql/bin/postgres</string>
 *		<string>-D</string>
 *		<string>/usr/local/pgsql/data</string>
 *	</array>
 *
 *  Exit Status:
 *	0	clean shutdown of postmaster with SIGINT
 *	1	unclean shutdown of postmaster with SIGKILL
 *	2	unexpected error
 *	3	unexpected exit of postmaster process
 *  Note:
 *	Convert to libjmscott.a
 *
 *	A simpler method must exist to send a SIGINT to a process started
 *	by launchd.
 */
#include <sys/wait.h>

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "jmscott/libjmscott.h"

extern int	errno;

static char	*pid_path = 0;
static int	SIGINT_pause = 0;
static pid_t	postmaster_pid = 0;
static int	exit_status = -1;

/*
 * Synopsis:
 *  	Fast, safe and simple string concatenator
 *  Usage:
 *  	buf[0] = 0
 *  	_strcat(buf, sizeof buf, "hello, world");
 *  	_strcat(buf, sizeof buf, ": ");
 *  	_strcat(buf, sizeof buf, "good bye, cruel world");
 */
static void
_strcat(char *tgt, int tgtsize, char *src)
{
	//  find null terminated end of target buffer
	while (*tgt++)
		--tgtsize;
	--tgt;

	//  copy non-null src bytes, leaving room for trailing null
	while (--tgtsize > 0 && *src)
		*tgt++ = *src++;

	// target always null terminated
	*tgt = 0;
}

/*
 *  Write info messages to standard error.
 */
static void
info(char *msg1)
{
	char msg[JMSCOTT_ATOMIC_WRITE_SIZE];
	static char nl[] = "\n";

	msg[0] = 0;

	snprintf(msg, sizeof msg, "pg_launchd: #%d: ", getpid());
	_strcat(msg, sizeof msg, msg1);
	_strcat(msg, sizeof msg, nl);

again:
	if (write(2, msg, strlen(msg)) < 0 && errno == EINTR)
		goto again;
}

static void
info2(char *msg1, char *msg2)
{
	char msg[JMSCOTT_ATOMIC_WRITE_SIZE];

	msg[0] = 0;
	_strcat(msg, sizeof msg, msg1);
	_strcat(msg, sizeof msg, ": ");
	_strcat(msg, sizeof msg, msg2);
	info(msg);
}

static void	die2(char *msg1, char *msg2);

static void
leave(int status)
{
	if (pid_path) {
		char *path = pid_path;
		pid_path = 0;
		info2("unlinking pid file", path);
		if (unlink(path) && errno != ENOENT)
			die2("unlink(pid path) failed", strerror(errno));
	}
	if (postmaster_pid > 0)
		info("good bye, cruel world");
	exit(status);
}

static void
ERROR(char *msg)
{
	info2("ERROR", msg);
}

static void
ERROR2(char *msg1, char *msg2)
{
	char msg[JMSCOTT_ATOMIC_WRITE_SIZE];

	msg[0] = 0;
	_strcat(msg, sizeof msg, msg1);
	_strcat(msg, sizeof msg, ": ");
	_strcat(msg, sizeof msg, msg2);
	ERROR(msg);
}

/*
 *  Write error message to standard error and exit process with status code.
 */
static void
die(char *msg)
{
	ERROR(msg);
	leave(1);
}

static void
die2(char *msg1, char *msg2)
{
	char msg[JMSCOTT_ATOMIC_WRITE_SIZE];

	msg[0] = 0;
	_strcat(msg, sizeof msg, msg1);
	_strcat(msg, sizeof msg, ": ");
	_strcat(msg, sizeof msg, msg2);

	die(msg);
}

static void
die3(char *msg1, char *msg2, char *msg3)
{
	char msg[JMSCOTT_ATOMIC_WRITE_SIZE];

	msg[0] = 0;
	_strcat(msg, sizeof msg, msg1);
	_strcat(msg, sizeof msg, ": ");
	_strcat(msg, sizeof msg, msg2);

	die2(msg, msg3);
}

static void
info_wait_status(int status)
{
	info("postmaster has stopped");

	/*
	 *  Unexpected exit of postmaster
	 */

	char what[JMSCOTT_ATOMIC_WRITE_SIZE];

	if (WIFSIGNALED(status))
		snprintf(what, sizeof what, "signaled: sig #%d", status);
	else if (WIFEXITED(status))
		snprintf(
			what,
			sizeof what,
			"exit status: %d",
			WEXITSTATUS(status)
		);
	else if (WIFSTOPPED(status))
		snprintf(what, sizeof what, "stopped: #%d", status);
	else
		snprintf(what, sizeof what, "unknown exit status: %d", status);
	info2("postmaster process terminated", what);
}

static void
WARN(char *msg)
{
	info2("WARN", msg);
}

static void
catch_TERM(int sig)
{
	(void)sig;

	info("signal TERM caught");

	if (!postmaster_pid) {
		WARN("postmaster not running");
		exit_status = 0;
		return;
	}

	//  fast shutdown with SIGINT to postmaster

	info("sending fast shutdown signal INT to postmaster ...");
	if (kill(postmaster_pid, SIGINT) != 0) {
		exit_status = 2;
		ERROR2("kill(postmaster, INT) failed", strerror(errno));
		return;
	}
	info("postmaster got signal");

	int pause = SIGINT_pause;
	SIGINT_pause = 0;
	while (pause > 1) {
		info("pausing 1 second as postmaster stops gracefully");
		sleep(1);
		--pause;

		int status;
		pid_t pid = waitpid(postmaster_pid, &status, WNOHANG);
		if (pid < 0) {
			exit_status = 2;
			ERROR2(
				"waitpid(postmaster TERM) failed",
				strerror(errno)
			);
			return;
		}

		//  postmaster exited
		if (pid > 0) {
			//  cheap sanity test
			if (pid != postmaster_pid) {
				exit_status = 2;
				ERROR("non postmaster process exited");
				return;
			}
			info_wait_status(status);
			exit_status = 0;
			return;
		}
	}

	//  nuke the postmaster

	WARN("postmaster will not stop");
	WARN("sending KILL signal to postmaster to force stop");
	if (kill(postmaster_pid, SIGKILL) && errno != ESRCH) {
		ERROR2("kill(postmaster, KILL) failed", strerror(errno));
		exit_status = 2;
		return;
	}
}

static void
eSIGINT(char *msg)
{
	die2("option: SIGINT-pause", msg);
}

static void
eSIGINT2(char *msg1, char *msg2)
{
	die3("option: SIGINT-pause", msg1, msg2);
}

static void
epid(char *msg)
{
	die2("option: pid-path", msg);
}

static void
postmaster(int pargc, char **pargv)
{
	pargv[pargc] = 0;

	postmaster_pid = fork();
	if (postmaster_pid < 0)
		die2("fork() failed", strerror(errno));

	if (postmaster_pid == 0) {
		info2("execv postmaster", pargv[0]);
		execv(pargv[0], pargv);
		die2("execv(postmaster) failed", strerror(errno));
	}
	signal(SIGTERM, catch_TERM);

	char buf[JMSCOTT_ATOMIC_WRITE_SIZE];

	snprintf(buf, sizeof buf, "postmaster pid: #%d", postmaster_pid);
	info(buf);
}

int
main(int argc, char **argv)
{
	int i;

	if (argc < 6)
		die("wrong number of command line arguments");

	char **pargv = (char **)malloc(argc * sizeof *argv);
	if (!pargv)
		die("malloc(argv) failed");

	int pargc = 0;
	for (i = 1;  i < argc;  i++)
		if (strcmp("--SIGINT-pause", argv[i]) == 0) {
			if (++i == argc)
				eSIGINT("missing seconds");
			if (SIGINT_pause)
				eSIGINT("option given more than once");

			char *pa = argv[i];
			errno = 0;
			SIGINT_pause = strtoul(pa, (char **)0, 10);
			if (SIGINT_pause == 0) {
				if (errno > 0)
					eSIGINT2(
						"can not parse seconds",
						strerror(errno)
					);
				else
					eSIGINT2(
						"can not parse seconds",
						argv[i]
					);
			}
		} else if (strcmp("--pid-path", argv[i]) == 0) {
			if (++i == argc)
				epid("missing path to pid file");
			if (pid_path)
				epid("option given more than once");
			pid_path = strdup(argv[i]);
		} else
			pargv[pargc++] = strdup(argv[i]);
	if (SIGINT_pause == 0)
		eSIGINT("missing required option");
	if (!pid_path)
		epid("missing required option");

	info("hello, world");

	postmaster(pargc, pargv);

	signal(SIGTERM, catch_TERM);

	int status = 0;

	if (waitpid(postmaster_pid, &status, 0) < 0 && errno != ECHILD)
		die2("waitpid(postmaster) failed", strerror(errno));

	//  error occured during TERM signal

	if (exit_status > -1)
		leave(exit_status);

	info_wait_status(status);

	leave(3);
	return 0;
}
