/*
 *  Synopsis:
 *	Select all tuples from pg_stat_activity
 *
 *  Command Line Variables:
 *	duration text
 *
 *  Usage:
 *	psql --file pg_stat_activity-slow-count.sql --set duration="'-1min'"
 */

SELECT
	count(*) as slow_count
  FROM
  	pg_catalog.pg_stat_activity
  WHERE
  	query_start <= (now() + :duration)
;
