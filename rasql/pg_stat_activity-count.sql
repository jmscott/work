/*
 *  Synopsis:
 *	Select all tuples from pg_stat_activity
 *
 *  Command Line Variables:
 *
 *  Usage:
 *	psql -f pg_stat_activity.sql	
 */

SELECT
	count(*) as "activity_count"
  FROM
  	pg_catalog.pg_stat_activity
;
