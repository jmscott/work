/*
 *  Synopsis:
 *	Interactive diff of 2 US_ASCII text files.
 *  Usage:
 *	idiff [-c diffcmd] [-o output] file1 file2
 *  History:
 *	Derived by John Scott from example in 'Unix Programming Environment' 
 *	by Brian Kernighan/Rob Pike.  Substantial additions by Andrew Fullford.
 *  Depends:
 *	Well known unix diff program
 *  Blame:
 *	jmscott@setspace.com
 *	https://github.com/jmscott/work
 *  Notes:
 *	The prompt doesn't appear to echo to terminal under Mac OSX 10.10.5.
 *	The pain never ends.
 */

static char title[] = "idiff, derived 'Unix Programming Environment Book'";

static char usage[] = "\n%s\n\
\n\
Usage: %s [-c diffcmd] [-o output] file1 file2\n\
\n\
       Interactively select file differences.\n\
       Default output file is \"%s\".\n\
       Default diff command is \"%s\".\n\
\n\
       For each difference group:\n\
         <  selects the lines from file1,\n\
         >  selects the lines from file2,\n\
         !  invokes a command, and\n\
         e  invokes an editor on the current group.\n\
            The result of the edit session is placed\n\
            in the diff output.\n";

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

static char default_output[] = "idiff.out";
static char default_diffcmd[] = "diff";
static char default_editor[] = "vi";

#define MAXLINE		10240		/* Maximum line length handled */

#ifdef NEED_STRERROR

extern int	sys_nerr;
extern char	*sys_errlist[];

static char *strerror(int err)
{
	static char msg[40];

        if (0 <= err && err < sys_nerr && sys_errlist[err])
                return sys_errlist[err];
	sprintf(msg, "Unknown error, errno = %d", err);
	return msg;
}
#endif /* NEED_STRERROR */

static char *prog;
static char *diffcmd;
static char *editor = 0;
static char *tempfile = 0;
static char temppath[128];
static char *ofile = 0;
static int child_pid;

/*
**  ascii to unsigned int
*/
#define a2i(s, p)	while (isdigit(*(s))) p = 10 * (p) + (*(s)++ - '0')

/*
**  Open file, otherwise complain and die
*/
static FILE *efopen(char *file, char *mode)
{
	FILE *fp;
	char *err;

	if (strcmp(file, "-") == 0)
		return *mode == 'r' ? stdin : stdout;

	if ((fp = fopen(file, mode)))
		return fp;

	err = strerror(errno);
	mode = *mode=='w'?"writing":*mode=='r'?"reading":
	       *mode=='a'?"appending":mode;
	fprintf(stderr, "%s: Can't open file '%s' for %s -- %s\n",
		prog, file, mode, err);

	exit(127);
}

/*
**  Skip n lines of file fin
*/
static void nskip(FILE *fin, int n)
{
	char buf[MAXLINE];

	while (n-- > 0)
		fgets(buf, sizeof buf,  fin);
}

/*
**  copy n lines from fin to fout.  If n == -1, copies to EOF.
*/
static void ncopy(FILE *fin, int n, FILE *fout)
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

static void parse
(
	char *s,
	int *pfrom1,
	int *pto1,
	int *pcmd,
	int *pfrom2,
	int *pto2
)
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

static void cleanup(int sig)
{
	signal(sig, SIG_IGN);

	if (tempfile) {
		unlink(tempfile);
		tempfile = 0;
	}
	if (sig) {
		if (child_pid)
			kill(child_pid, SIGTERM);
		if (ofile && strcmp(ofile, "-") != 0) {
			unlink(ofile);
			fprintf(stderr,
				"\n%s: Interrupted -- \"%s\" removed\n",
				prog, ofile);
		}
	}
	exit(sig);
}

static void idiff(FILE *f1, FILE *f2, FILE *fin, FILE *fout)
{
	char buf[MAXLINE], ed[BUFSIZ];
	FILE *ft;
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
			case 'e':
				strcpy(temppath, "/tmp/idiff.XXXXXX");

				if (!tempfile)
					tempfile = mktemp(temppath);
				if (!tempfile) {
					fprintf(stderr,
					  "%s: could not construct temp file\n",
					  prog);
					exit(127);
				}
				if (!editor) {
					editor = getenv("EDITOR");
					if (!editor)
						editor = default_editor;
				}
				ncopy(f1, from1 - 1 - nf1, fout);
				nskip(f2, from2 - 1 - nf2);
				ft = efopen(tempfile, "w");
				ncopy(f1, to1 + 1 - from1, ft);
				fprintf(ft, "---\n");
				ncopy(f2, to2 + 1 - from2, ft);
				fclose(ft);
				sprintf(ed, "%s %s", editor, tempfile);
				system(ed);
				ft = efopen(tempfile, "r");
				ncopy(ft, -1, fout);
				fclose(ft);
				unlink(tempfile);
				break;
			
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

static FILE *efdiff(char *in1, char *in2)
{
	int fds[2];
	FILE *fp;
	char cmd[BUFSIZ];

	if (pipe(fds) != 0) {
		fprintf(stderr, "%s: Pipe failed -- %s\n",
			prog, strerror(errno));
		exit(127);
	}

	child_pid = fork();
	switch(child_pid) {
	case -1:
		fprintf(stderr, "%s: Fork failed -- %s\n",
			prog, strerror(errno));
		exit(127);
	case 0:
		/*
		**  Adjust standard files.  Attach stdout to pipe and
		**  stdin to /dev/null.
		*/
		close(1);
		if (dup(fds[1]) != 1) {
			fprintf(stderr, "%s: Unexpected dup result\n", prog);
			exit(126);
		}
		close(0);
		open("/dev/null", O_RDONLY, 0);
		close(fds[0]);
		close(fds[1]);

		/*
		**  Exec via a shell so that "diffcmd" is interpreted.
		**  A command line arg might try magic.
		*/
		sprintf(cmd, "exec %s %s %s", diffcmd, in1, in2);
		execlp("sh", "sh", "-c", cmd, (char *)0);

		fprintf(stderr,
			"%s: Exec of <sh -c \"%s\"> command failed -- %s\n",
			prog, cmd, strerror(errno));
		exit(126);
	default:
		close(fds[1]);
		fp = fdopen(fds[0], "r");
		if (!fp) {
			fprintf(stderr, "%s: Fdopen failed after fork -- %s\n",
				prog, strerror(errno));
			cleanup(127);
		}
		return fp;
	}
}

int
main(int argc, char **argv)
{
	FILE *fin, *fout, *f1, *f2, *efopen();
	int c, err;
	char *in1, *in2;
	struct stat s1, s2, so;
	extern char *optarg;
	extern int optind;

	if ((prog = strrchr(*argv, '/')))
		prog++;
	else
		prog = *argv;

	err = 0;
	diffcmd = 0;
	ofile = 0;
	while ((c = getopt(argc, argv, "c:o:")) != -1) {
		switch(c) {
		case 'c':
			diffcmd = optarg;
			break;
		case 'o':
			ofile = optarg;
			break;
		default:
			err = 1;
			break;
		}
	}

	if (err || argc - optind != 2) {
		fprintf(stderr, usage,
			title, prog, default_output, default_diffcmd);
		exit(127);
	}

	if (!isatty(0)) {
		fprintf(stderr, "%s: Input is not attached to a terminal\n",
			prog);
		exit(127);
	}

	if (!diffcmd)
		diffcmd = default_diffcmd;
	if (!ofile)
		ofile = default_output;

	in1 = argv[optind];
	in2 = argv[optind+1];

	f1 = efopen(in1, "r");
	f2 = efopen(in2, "r");

	if (fstat(fileno(f1), &s1) != 0) {
		fprintf(stderr, "%s: Can't stat input file \"%s\" -- %s\n",
			prog, in1, strerror(errno));
		exit(127);
	}
	if (fstat(fileno(f2), &s2) != 0) {
		fprintf(stderr, "%s: Can't stat input file \"%s\" -- %s\n",
			prog, in2, strerror(errno));
		exit(127);
	}
	if (!S_ISREG(s1.st_mode) && !S_ISFIFO(s1.st_mode)) {
		fprintf(stderr, "%s: Input file \"%s\" is not a regular file\n",
			prog, in1);
		exit(127);
	}
	if (!S_ISREG(s2.st_mode) && !S_ISFIFO(s2.st_mode)) {
		fprintf(stderr, "%s: Input file \"%s\" is not a regular file\n",
			prog, in2);
		exit(127);
	}
	if (s1.st_ino == s2.st_ino && s1.st_dev == s2.st_dev) {
		fprintf(stderr,
			"%s: Input files \"%s\" and \"%s\" are identical\n",
			prog, in1, in2);
		exit(127);
	}

	if (strcmp(ofile, "-") == 0) {
		/*
		**  If the output file is stdout we have to play some
		**  games because an editor may be started and it will
		**  expect stdout to be a terminal.  In this case, stdout
		**  is duped for output and stdin is duped to stdout.
		*/
		int fd;

		fd = dup(1);
		close(1);
		if (dup(0) != 1) {
			fprintf(stderr,
				"%s: Bad dup result transfering stdout\n",
				prog);
			exit(127);
		}
		fout = fdopen(fd, "w");
		if (!fout) {
			fprintf(stderr, "%s: Fdopen failed for output -- %s\n",
				prog, strerror(errno));
			cleanup(127);
		}
	} else {
		if (stat(ofile, &so) == 0) {
			if (so.st_ino == s1.st_ino && so.st_dev == s1.st_dev) {
				fprintf(stderr,
				    "%s: Output \"%s\" would clobber \"%s\"\n",
				    prog, ofile, in1);
				exit(127);
			}
			if (so.st_ino == s2.st_ino && so.st_dev == s2.st_dev) {
				fprintf(stderr,
				    "%s: Output \"%s\" would clobber \"%s\"\n",
				    prog, ofile, in2);
				exit(127);
			}
		}
		fout = efopen(ofile, "w");
	}

	signal(SIGINT, cleanup);
	signal(SIGHUP, cleanup);
	signal(SIGTERM, cleanup);

	fin = efdiff(in1, in2);

	idiff(f1, f2, fin, fout);

	printf("%s output in file \"%s\"\n", prog, ofile);

	cleanup(0);
	return 0;
}
