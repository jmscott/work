#  FURL - Fault URL - A Trivial Monitor of Curlable URLS

##  Description

FURL stands for "Fault URL".  Furl can be used in trivial monitoring
environments that don't need the overhead of 
Using the well known "curl" program
furl will fetch a url and go into a fault state upon failure.
When entering the fault state will notify the hords once and not
send another email until a fault file is removed in the
$FURL_ROOT/spool directory, indicating the fault has cleared.
After the fault has cleared, a second notice is emailed to the hordes,
indicating that the fault has cleared.

Typically furl is invoked from a cron jobs.
