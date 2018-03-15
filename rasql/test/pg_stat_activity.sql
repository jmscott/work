/*
 *  Synopsis:
 *	Select attributes for processes in table pg_catalog.pg_stat_activity.
 *  Description:
 *	Select attributes for processes in table pg_catalog.pg_stat_activity.
 *	The rows are ascendingly ordered by the database name, the process
 *	state, the application name and the query start time.
 *
 *  Command Line Variables:
 *
 *  Usage:
 *	psql -f pg_stat_activity.sql	
 */

SELECT
	datid,
	datname,
	pid,
	usesysid,
	usename,
	application_name,
	client_addr,
	client_hostname,
	client_port,
	backend_start,
	xact_start,
	query_start,
	state_change,
	wait_event_type,
	wait_event,
	state,
	backend_xid,
	backend_xmin,
	query
  FROM
  	pg_catalog.pg_stat_activity
  ORDER BY
  	datname ASC,
	state ASC,
	application_name ASC,
	query_start ASC
;
