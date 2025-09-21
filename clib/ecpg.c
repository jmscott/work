/*
 *  Synopsis:
 *	Helpful routines for simple PostgreSQL *.pgc programs.
 *  Usage:
 *	#include "jmscott/libjmscott.h"
 *
 *	static struct jmscott_ecpg_state_fault no_warn[] =
 *	{
 *		{"02000", 0},         //  no data for conflict on upsert/insert
 *		{(char *)0, 0}
 *	};
 *	...
 *	
 *	EXEC SQL WHENEVER SQLWARNING CALL jmscott_ecpg_warning(
 *		no_warn
 *	);
 *  Note:
 *	Should struct jmscott_ecpg_state_fault{} contain the exit codes,
 *	instead of declaring vars jmscott_ecpg_{error,warn}_code global?
 *
 *	Need to make code reentrant, via a jmscott_clib data structure!
 */

#include <unistd.h>
#include <string.h>

#if JMSCOTT_COMPILE_PG
#include "sqlca.h"
#endif

#include "jmscott/libjmscott.h"

//  default values when fault map has not mapping for a particular
//  sql error code
int	jmscott_ecpg_error_code = 127;
int	jmscott_ecpg_warn_code = 126;

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
#if JMSCOTT_COMPILE_PG
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
#else
	(void)status;
	(void)what;
	(void)fault;
	jmscott_die(127, "jmscott_ecpg_fault: not compiled with postgresql");
#endif
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
