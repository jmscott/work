/*
 *  Synopsis:
 *	Helpful routines for compile simple PostgreSQL *.ecpg programs.
 *  Note:
 *	Need to make code reentrant, via a jmscott_clang data structure!
 */

#ifndef JMSCOTT_CLANG_ECPG
#define JMSCOTT_CLANG_ECPG

#include "sqlca.h"

#include "jmscott/die.c"

//  default values when fault map has not mapping for a particular
//  sql error code
int	jmscott_ecpg_error_code = 127;
int	jmscott_ecpg_warn_code = 126;

//  map sql state codes onto process exit status.
//  only used when query faults.  caller code
//  defines mapping.

struct jmscott_ecpg_state_fault
{
	char	*sql_state;

	// -1 implies ignore fault, 0 <= 255 implies remap exit status

	int	action;
};

/*
 *  Synopsis:
 *	Low level handler for all sql faults that end with jmscott_die().
 */
void
jmscott_ecpg_fault(
	int status,
	char *what,
	struct jmscott_ecpg_state_fault *fault
) {
	char msg[4096];

	msg[0] = 0;
	jmscott_strcat(msg, sizeof msg, "SQL");
	jmscott_strcat(msg, sizeof msg, what);

	//  add the sql state code to error message

	if (sqlca.sqlstate[0] != 0) {
		char state[6];

		jmscott_strcat(msg, sizeof msg, ": ");
		memmove(state, sqlca.sqlstate, 5);
		state[5] = 0;
		jmscott_strcat(msg, sizeof msg, state);

		//  remap the process exit code for a particular sql state code

		if (fault) {
			while (fault->sql_state) {
				if (strcmp(state, fault->sql_state) == 0)
					break;
				fault++;
			}

			//  change the default behavior of the fault based
			//  the sql state code

			if (fault->sql_state) {
				int act = fault->action;

				//  ignore the fault

				if (act == -1)
					return;

				//  change the process exit status

				if (0 <= act && act <= 255)
					status = act;
			}
		}
	} else
		jmscott_strcat(msg, sizeof msg, ": (WARN: missing sql state)");

	//  tack on the sql error message

	if (sqlca.sqlerrm.sqlerrml > 0) {
		char err[SQLERRMC_LEN + 1];

		jmscott_strcat(msg, sizeof msg, ": ");
		memmove(err, sqlca.sqlerrm.sqlerrmc, sqlca.sqlerrm.sqlerrml);
		err[sqlca.sqlerrm.sqlerrml] = 0;
		jmscott_strcat(msg, sizeof msg, err);
	}
	jmscott_die(status, msg);
}

/*
 *  Synopsis:
 *	SQL error callback for EXEC SQL WHENEVER SQLERROR CALL ...
 */
void
jmscott_ecpg_error(struct jmscott_ecpg_state_fault *fault)
{
	jmscott_ecpg_fault(jmscott_ecpg_error_code, "ERROR", fault);
}

/*
*  Synopsis:
*	SQL warning callback for EXEC SQL WHENEVER SQLWARNING CALL
*  Usage:
*	#include "jmscott/lib/clang/ecpg.c"
*
*	..
*	static struct jmscott_ecpg_state_fault no_warn[] =
*	{
*		{"02000", 0},	//  no data for conflict on upsert/insert
*        	{(char *)0, 0}
*	};
*
*	...
*	
*	EXEC SQL WHENEVER SQLWARNING CALL
*		jmscott_ecpg_warning(no_warn);
*/
void
jmscott_ecpg_warning(struct jmscott_ecpg_state_fault *fault)
{
	jmscott_ecpg_fault(jmscott_ecpg_warn_code, "WARNING", fault);
}

#endif	//  define JMSCOTT_CLANG_ECPG
