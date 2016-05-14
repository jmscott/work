/*
 * Synopsis:
 *   	Trivial benchmark test of fork/exec.
 *  Usage:
 *	time fork-bench /usr/bin/true
 */
#include <sys/wait.h>

#include <unistd.h>
#include <stdio.h>

int
main(int argc, char **argv)
{
	int i = 0;
	pid_t pid;
	int status;
	char *path;
	static char blurb[] = "fork-bench: starting 100000 fork/execs ...\n";
	static char done[] = "fork-bench: done\n";

	if (argc != 2) {
		static char badargc[] = "ERROR: wrong number of arguments\n";

		write(2, badargc, sizeof badargc - 1);
		_exit(1);
	}

	write(1, blurb, sizeof blurb - 1);

	path = argv[1];
	while (i++ < 100000) {

		pid = fork();
		if (pid == 0) {
			execl(path, path, NULL);
			perror(path);
			_exit(1);
		}
		if (pid < 0) {
			perror("fork() failed");
			_exit(1);
		}

		if (wait(&status) < 0) {
			perror("wait() failed");
			_exit(1);
		}
		if (WIFEXITED(status)) {
			if (status != 0) {
				static char notzero[] =
					"ERROR: child exit status not 0\n";

				write(2, notzero, sizeof notzero - 1);
				_exit(1);
			}
		} else {
			static char badkid[] =
				"ERROR: child process did not exit normally\n";
			write(2, badkid, sizeof badkid - 1);
			_exit(1);
		}
	}
	write(1, done, sizeof done - 1);
	return 0;
}
