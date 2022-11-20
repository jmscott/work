/*
 *  Synopsis:
 *	Write error message to standard error and exit process with status code.
 *  Usage:
 *	#include "jmscott/libjmscott.h"
 *
 *	static void
 *	die(char msg) {
 *		jmscott_die(127, msg);
 *	}
 *  Note:
 *	Consider renaming file die.c to exit.c
 *
 *	Should "exit()" be called instaead of "_exit()"?  Perhaps stdio is
 *	being used.  What happens if stdio buffers not flushed, etc?
 */

#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "jmscott/libjmscott.h"

extern char	*jmscott_progname;

void
jmscott_die(int status, char *msg1)
{
	char msg[JMSCOTT_ATOMIC_WRITE_SIZE];

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
        char msg[JMSCOTT_ATOMIC_WRITE_SIZE];

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
        char msg[JMSCOTT_ATOMIC_WRITE_SIZE];

        msg[0] = 0;
        jmscott_strcat(msg, sizeof msg, msg1);
        jmscott_strcat(msg, sizeof msg, colon);
        jmscott_strcat(msg, sizeof msg, msg2);

        jmscott_die2(status, msg, msg3);
}

void
jmscott_die4(int status, char *msg1, char *msg2, char *msg3, char *msg4)
{
        static char colon[] = ": ";
        char msg[JMSCOTT_ATOMIC_WRITE_SIZE];

        msg[0] = 0;
        jmscott_strcat(msg, sizeof msg, msg1);
        jmscott_strcat(msg, sizeof msg, colon);
        jmscott_strcat(msg, sizeof msg, msg2);

        jmscott_die3(status, msg, msg3, msg4);
}

void
jmscott_die5(
	int status,
	char *msg1,
	char *msg2,
	char *msg3,
	char *msg4,
	char *msg5
){
        static char colon[] = ": ";
        char msg[JMSCOTT_ATOMIC_WRITE_SIZE];

        msg[0] = 0;
        jmscott_strcat(msg, sizeof msg, msg1);
        jmscott_strcat(msg, sizeof msg, colon);
        jmscott_strcat(msg, sizeof msg, msg2);

        jmscott_die4(status, msg, msg3, msg4, msg5);
}

void
jmscott_die6(
	int status,
	char *msg1,
	char *msg2,
	char *msg3,
	char *msg4,
	char *msg5,
	char *msg6
) {
        static char colon[] = ": ";
        char msg[JMSCOTT_ATOMIC_WRITE_SIZE];

        msg[0] = 0;
        jmscott_strcat(msg, sizeof msg, msg1);
        jmscott_strcat(msg, sizeof msg, colon);
        jmscott_strcat(msg, sizeof msg, msg2);

        jmscott_die5(status, msg, msg3, msg4, msg5, msg6);
}

void
jmscott_die_argc(int status, int got, int expect, char *usage)
{
	char msg[JMSCOTT_ATOMIC_WRITE_SIZE] = {0};
	char suffix[54] = {0};

	jmscott_strcat(msg, sizeof msg, "wrong number of cli args: got ");
	snprintf(suffix, sizeof suffix, "%d, expected %d: ", got, expect);
	jmscott_strcat(msg, sizeof msg, suffix);
	jmscott_strcat2(msg, sizeof msg, "\nusage: ", usage);

	jmscott_die(status, msg);
}
