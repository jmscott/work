/*
 *  Synopsis:
 *	Write error message to standard error and exit process with status code.
 *  Usage:
 *	#include "jmscott/lib/clang/die.c"
 *
 *	static void
 *	die(char msg) {
 *		jmscott_die(127, msg);
 *	}
 *  Note:
 *	Consider renaming file die.c to exit.c
 *
 *	Should "exit()" be called instaead of "_exit()"?  Perhaps stdio is
 *	being used.
 */

#include <unistd.h>
#include <string.h>

#include "jmscott/libjmscott.h"

extern char	*jmscott_progname;

void
jmscott_die(int status, char *msg1)
{
	char msg[4096];

        static char ERROR[] = "ERROR: ";
        static char colon[] = ": ";
        static char nl[] = "\n";

        msg[0] = 0;
	if (jmscott_progname) {
		jmscott_strcat(msg, sizeof msg, jmscott_progname);
		jmscott_strcat(msg, sizeof msg, colon);
	}
        jmscott_strcat(msg, sizeof msg, ERROR);
        jmscott_strcat(msg, sizeof msg, msg1);
        jmscott_strcat(msg, sizeof msg, nl);

        write(2, msg, strlen(msg));

        _exit(status);
}

void
jmscott_die2(int status, char *msg1, char *msg2)
{
        static char colon[] = ": ";
        char msg[4096];

        msg[0] = 0;
        jmscott_strcat(msg, sizeof msg, msg1);
        jmscott_strcat(msg, sizeof msg, colon);
        jmscott_strcat(msg, sizeof msg, msg2);

        jmscott_die(status, msg);
}

void
jmscott_die3(int status, char *msg1, char *msg2, char *msg3)
{
        static char colon[] = ": ";
        char msg[4096];

        msg[0] = 0;
        jmscott_strcat(msg, sizeof msg, msg1);
        jmscott_strcat(msg, sizeof msg, colon);
        jmscott_strcat(msg, sizeof msg, msg2);

        jmscott_die2(status, msg, msg3);
}
