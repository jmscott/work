/*
 *  Synopsis:
 *	Select all queries in table pg_stat_activity.
 *
 *  Command Line Variables:
 *
 *  Usage:
 *	psql -f pg_stat_activity-count.sql
 */

SELECT
	count(*) as "activity_count"
  FROM
  	pg_catalog.pg_stat_activity
;
