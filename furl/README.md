#  FURL - Fault URL - A Trivial Monitor of Curlable URLS

##  Description

FURL stands for "Fault URL".  Furl can be used in trivial monitoring
environments that don't need the overhead of
[SNPP](http://en.wikipedia.org/wiki/Simple_Network_Management_Protocol).
The monitor rules to furl can set on the command line of a cronjob.

Using the well known [curl](http://en.wikipedia.org/wiki/CURL) program,
furl will fetch a url and go into a fault state upon failure.
When entering the fault state furl will notify the hords once and not
send another email until a fault file is removed in the
`$FURL_ROOT/spool` directory, indicating the fault has cleared.
After the fault has cleared, a second notice is emailed to the hordes,
indicating that the fault has cleared.

##  Usage
```crontab
#
#  What:
#	Poll esdreports.esdallas.org
#  When:
#	Every 10 minutes
#
FURL_NOTIFY=jmscott@setspace.com

*/10 * * * * bin/furl --tag abcreports --url http://abcreports.abcallas.org $FURL_NOTIFY --curl-option '--user jmscott:lose20pd' >>log/abcreports.log 2>&1
```
