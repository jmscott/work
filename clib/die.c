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
 *	Consider die() NOT exiting if status == 0 or == 256.
 *
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

int jmscott_panic_exit_status = 70;	//  EX_SOFTWARE;

void
jmscott_die(int status, char *msg1)
{
	char msg[JMSCOTT_ATOMIC_WRITE_SIZE];

        static char ERROR[] = "ERROR: ";
        static char colon[] = ": ";
        static char nl[] = "\n";

        msg[0] = 0;
	if (jmscott_progname)
		jmscott_strcat2(msg, sizeof msg, jmscott_progname, colon);
	jmscott_strcat3(msg, sizeof msg, ERROR, msg1, nl);

        write(2, msg, strlen(msg));

        _exit(status);
}

void
jmscott_die2(int status, char *msg1, char *msg2)
{
        static char colon[] = ": ";
        char msg[JMSCOTT_ATOMIC_WRITE_SIZE];

        msg[0] = 0;
	jmscott_strcat3(msg, sizeof msg, msg1, colon, msg2);
        jmscott_die(status, msg);
}

void
jmscott_die3(int status, char *msg1, char *msg2, char *msg3)
{
        static char colon[] = ": ";
        char msg[JMSCOTT_ATOMIC_WRITE_SIZE];

        msg[0] = 0;
	jmscott_strcat3(msg, sizeof msg, msg1, colon, msg2);

        jmscott_die2(status, msg, msg3);
}

void
jmscott_die4(int status, char *msg1, char *msg2, char *msg3, char *msg4)
{
        static char colon[] = ": ";
        char msg[JMSCOTT_ATOMIC_WRITE_SIZE];

        msg[0] = 0;
	jmscott_strcat3(msg, sizeof msg, msg1, colon, msg2);

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
	jmscott_strcat3(msg, sizeof msg, msg1, colon, msg2);

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
	jmscott_strcat3(msg, sizeof msg, msg1, colon, msg2);

        jmscott_die5(status, msg, msg3, msg4, msg5, msg6);
}

void
jmscott_die_argc(int status, int got, int expect, char *usage)
{
	char suffix[54] = {0};
	char msg[JMSCOTT_ATOMIC_WRITE_SIZE] = {0};

	snprintf(suffix, sizeof suffix, "%d, expected %d: ", got, expect);

	msg[0] = 0;
	jmscott_strcat4(msg, sizeof msg,
			"wrong number of cli args: got ",
			suffix,
			"\nusage: ",
			usage
	);
	jmscott_die(status, msg);
}

void
jmscott_panic(char *msg)
{
	char *preamble = "jmslib: ERROR: PANIC: ";

	write(2, "\n", 1);
	if (jmscott_progname) {
		write(2, jmscott_progname, strlen(jmscott_progname));
		write(2, ": ", 2);
	}
	write(2, preamble, strlen(preamble));
	write(2, msg, strlen(msg));
	write(2, "\n", 1);
}

void
jmscott_panic2(char *msg1, char *msg2)
{
	char *preamble = "jmslib: ERROR: PANIC: ";

	write(2, "\n", 1);
	if (jmscott_progname) {
		write(2, jmscott_progname, strlen(jmscott_progname));
		write(2, ": ", 2);
	}
	write(2, preamble, strlen(preamble));
	write(2, msg1, strlen(msg1));
	write(2, ": ", 2);
	write(2, msg2, strlen(msg2));
	write(2, "\n", 1);
}
