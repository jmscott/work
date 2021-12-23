/*
 *  Synopsis:
 *	Interactive diff of two US_ASCII text files, inspiration from Pike book.
 *  Usage:
 *	idiff file1 file2
 *  Exit Status:
 *	0	gracefull exit
 *	<sig>	signal of interupted idiff
 *	127	unknown error
 *  History:
 *	Derived by John Scott from example in 'Unix Programming Environment' 
 *	by Brian Kernighan/Rob Pike.  Substantial additions by Andrew Fullford.
 *  Depends:
 *	Output of well known unix diff program, both BSD and GNU versions.
 *  Blame:
 *	jmscott@setspace.com
 *	https://github.com/jmscott/work
 *  See:
 *	https://github.com/jmscott/work/blob/master/idiff.c
 *  Notes:
 *	A sporadic bug still exists when ^D exiting.  Difficult to repeat.
 *
 *	UTF-8 not tested.
 */

/*
**  Compiling and linking:
**
**  The code is Unix dependent and makes use of the standard Unix
**  diff program.  It should compile on any recent Unix systems
**  which have an ISO C compiler.  Older Suns may need -DNEED_STRERROR.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define MAXLINE		10240		/* Maximum line length handled */

static char *prog;
static char tempfile[BUFSIZ] = {0};
static int child_pid;

/*
**  ascii to unsigned int
*/
#define a2i(s, p)	while (isdigit(*(s))) p = 10 * (p) + (*(s)++ - '0')

static void
leave(int sig)
{
	signal(sig, SIG_IGN);

	if (tempfile[0]) {
		unlink(tempfile);
		tempfile[0] = 0;
	}
	if (sig && child_pid)
		kill(child_pid, SIGTERM);
	exit(sig);
}

static void
usage(int fd)
{
static char blurb[] = "\n\
Usage: idiff file1 file2\n\
\n\
       Interactively select file differences and write result to idiff.out\n\
\n\
       For each difference group:\n\
         <  selects the lines from file1,\n\
         >  selects the lines from file2,\n\
         !  invokes a command, and\n\
         e  invokes an $EDITOR or nano on the current diff group.\n\
            The result of the edit session is placed\n\
            in the diff output.\n"
;
	write(fd, blurb, strlen(blurb));
}

static void
die(char *msg)
{
	char buf[MAXLINE];

	strcpy(buf, prog);
	strcat(buf, ": ERROR: ");
	strncat(buf, msg, sizeof buf - (strlen(buf) + 2));
	strncat(buf, "\n", sizeof buf - (strlen(buf) + 2));

	buf[sizeof buf - 2] = '\n';
	buf[sizeof buf - 1] = 0;
	write(2, buf, strlen(buf)); 

	usage(2);

	leave(127);
}

static void
die2(char *msg1, char *msg2)
{
	char buf[MAXLINE];

	strncpy(buf, msg1, sizeof buf - 1);
	strncat(buf, ": ", sizeof buf - (strlen(buf) + 1));
	strncat(buf, msg2, sizeof buf - (strlen(buf) + 1));
	die(buf);
}

static void
edie(char *msg)
{
	die2(msg, strerror(errno));
}

static FILE *
_fopen(char *file, char *mode)
{
	FILE *fp;
	char buf[MAXLINE];

	if (strcmp(file, "-") == 0)
		return *mode == 'r' ? stdin : stdout;

	if ((fp = fopen(file, mode)))
		return fp;
	snprintf(buf, sizeof buf, "fopen(%s/%s) failed", mode, file);
	edie(buf);

	return (FILE *)0;		//  not reached
}

/*
**  Skip n lines of file fin
*/
static void
nskip(FILE *fin, int n)
{
	char buf[MAXLINE];

	while (n-- > 0)
		fgets(buf, sizeof buf,  fin);
}

/*
**  copy n lines from fin to fout.  If n == -1, copies to EOF.
*/
static void
ncopy(FILE *fin, int n, FILE *fout)
{
	char buf[MAXLINE];

	if (n == -1) {
		while (fgets(buf, sizeof buf, fin))		
			fputs(buf, fout);
	} else while (n-- > 0) {
		if (!fgets(buf, sizeof buf, fin))		
			return;
		fputs(buf, fout);
	}
}

//  parse output from diff command
static void
parse(char *s, int *pfrom1, int *pto1, int *pcmd, int *pfrom2, int *pto2)
{
	*pfrom1 = *pto1 = *pfrom2 = *pto2 = 0;
	a2i(s, *pfrom1);
	if (*s == ',') {
		s++;
		a2i(s, *pto1);
	} else
		*pto1 = *pfrom1;
	*pcmd = *s++;
	a2i(s, *pfrom2);
	if (*s == ',') {
		s++;
		a2i(s, *pto2);
	} else
		*pto2 = *pfrom2;
}

static void
idiff(FILE *f1, FILE *f2, FILE *fin, FILE *fout)
{
	char buf[MAXLINE], ed[2 * BUFSIZ];
	int cmd, n, from1, to1, from2, to2, nf1, nf2, again;

	nf1 = nf2 = 0;
	while (fgets(buf, sizeof buf, fin)) {
		parse(buf, &from1, &to1, &cmd, &from2, &to2);

		n = to1 - from1 + to2 - from2 + 1;  	/* Lines from diff */
		if (cmd == 'c')
			n += 2;
		else if (cmd == 'a')
			from1++;
		else if (cmd == 'd')
			from2++;
		printf("%s", buf); fflush(stdout);

		while (n-- > 0) {
			fgets(buf, sizeof buf, fin);
			printf("%s", buf);
		}

		again = 0;
		do {
			printf("? "); fflush(stdout);
			fgets(buf, sizeof buf, stdin);
			switch(buf[0]) {
			case '>':
				nskip(f1, to1 - nf1);
				ncopy(f2, to2 - nf2, fout);
				break;

			case '<':
				nskip(f2, to2 - nf2);
				ncopy(f1, to1 - nf1, fout);
				break;

			case 'v':
			case 'e': {
				/*
				 *  Note:
				 *	Which of the goofy temp functions in
				 *	posix simply gives me a temp path,
				 *	not a FILE *.  I can not reopen
				 *	a FILE * for reading!
				 */ 
				char *TMPDIR;
				TMPDIR = getenv("TMPDIR");
				if (TMPDIR == NULL)
					TMPDIR = "/tmp";

				snprintf(tempfile, sizeof tempfile - 1,
					"%s/idiff-%ld-%ld",
					TMPDIR,
					(long)getpid(),
					(long)random()
				);
				FILE *ft = _fopen(tempfile, "w");

				char *EDITOR = getenv("EDITOR");
				if (EDITOR == NULL)
					EDITOR = "nano";
				ncopy(f1, from1 - 1 - nf1, fout);
				nskip(f2, from2 - 1 - nf2);
				ncopy(f1, to1 + 1 - from1, ft);
				fprintf(ft, "---\n");
				ncopy(f2, to2 + 1 - from2, ft);
				fclose(ft);
				snprintf(ed, sizeof ed - 1,
					"%s %s",
					EDITOR,
					tempfile
				);
				system(ed);
				ft = _fopen(tempfile, "r");
				ncopy(ft, -1, fout);
				fclose(ft);
				unlink(tempfile);
				tempfile[0] = 0;
			}	break;
			
			case '!':
				system(buf + 1);
				printf("!\n");
				break;
			default:
				printf("< or > or e or !\n");
				again = 1;
				break;
			}
		} while (again);
		nf1 = to1;
		nf2 = to2;
	}
	ncopy(f1, -1, fout);
}

static FILE *
efdiff(char *in1, char *in2)
{
	int fds[2];
	FILE *fp;
	char cmd[BUFSIZ];

	if (pipe(fds) != 0)
		edie("pipe(diff) failed");

	child_pid = fork();
	switch (child_pid) {
	case -1:
		edie("fork(diff) failed");
		//  not reached */
	case 0:
		/*
		**  Adjust standard files.  Attach stdout to pipe and
		**  stdin to /dev/null.
		*/
		close(1);
		if (dup(fds[1]) != 1)
			edie("dup(diff) failed");
		close(0);
		open("/dev/null", O_RDONLY, 0);
		close(fds[0]);
		close(fds[1]);

		/*
		**  Exec via a shell so that "diffcmd" is interpreted.
		**  A command line arg might try magic.
		*/
		sprintf(cmd, "exec diff %s %s", in1, in2);
		execlp("sh", "sh", "-c", cmd, (char *)0);

		edie("exec(diff) failed");
	default:
		close(fds[1]);
		fp = fdopen(fds[0], "r");
		if (!fp)
			edie("fdopen(fork diff)");
		return fp;
	}
}

int
main(int argc, char **argv)
{
	FILE *fin, *fout, *f1, *f2, *_fopen();
	char *in1, *in2;
	struct stat s1, s2;
	extern char *optarg;
	extern int optind;

	if (argc <= 1 || strcmp(argv[1], "--help") == 0) {
		usage(1);
		exit(0);
	}
	if (argc != 3)
		die("expected 2 command line arguments");

	if (!isatty(0))
		die("input is not a terminal");

	in1 = argv[1];
	in2 = argv[2];

	f1 = _fopen(in1, "r");
	f2 = _fopen(in2, "r");

	if (fstat(fileno(f1), &s1) != 0)
		edie("stat(file 1) failed");
	if (fstat(fileno(f2), &s2) != 0)
		edie("stat(file 2) failed");
	if (!S_ISREG(s1.st_mode) && !S_ISFIFO(s1.st_mode))
		die2("file 1 is not a regular file", in1);
	if (!S_ISREG(s2.st_mode) && !S_ISFIFO(s2.st_mode))
		die2("file 2 is not a regular file", in2);
	if (s1.st_ino == s2.st_ino && s1.st_dev == s2.st_dev)
		die("input files are identical");

	//  could clobber existing idiff.out!
	fout = _fopen("idiff.out", "w");

	signal(SIGINT, leave);
	signal(SIGHUP, leave);
	signal(SIGTERM, leave);

	fin = efdiff(in1, in2);

	idiff(f1, f2, fin, fout);

	printf("output in file idiff.out\n");

	leave(0);
	return 0;	//  not reached
}
