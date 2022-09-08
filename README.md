#  Code Used by Customers of John Scott

## Install dependencies

###  google golang, expected in /usr/local/go
	macosx: dmg installer on golang.org
	linux: tar install in /usr/local/go
###  perl package XML::Parser
	debian: apt-get libxml-parser-perl (linux)
	macosx: port install  port install p5-xml-parser

###  perl package postgres DBI driver
	macosx: port install p5-dbd-pg
	debian: apt-get install libdbd-pg-perl

##  Tarballable Packages

### rasql
	REST api built by parsing structured comments in PostgreSQL SQL files

###  pgsnap
	Snapshot postgresql database with pg_dump and notify the hordes

###  furl
	Fault a failed URL fetch and alert the hordes on fault|clear state change.

###  logroll
	Roll log files via Day of Week or Sample Rate, written in go.

### www
	Various tools used for apache/ngix web development.

## Standalone Shell Scripts

###  pg2pg
	Upgrade a postgresql database via pg_upgrade or pg_dump

###  flatx
	Flatten XML for simple extraction with shell scripts.

###  hexdump
	C function to format like command line tool 'hexdump -C'

###  isjson
	Is input valid json, based up python lib json.tool

###  launchd-log
	Trivial wrapper script called by mac launchd to do decent logging

###  pdf-merge
	Merge pdf files calling java classes in Apache pdfbox.

###  make-dist
	Build a tar distribution using a simple config file.

###  ssh-host
	SSH to host, where host name is derived from symbolic link to ssh-host.

###  rsunk
	Push files to rsync.net public service and notify hordes.

###  rsync-fs
	rsync a whole file system, notify hordes.

###  overwrite
	Safely overwrite files (based on rob & bwk script).

###  strswap
	Swap string in files (based on rob & bwk script).

###  strip-cr
	String dos style carriage return/newline sequences

###  xtitle
	Set title of XTerm session.

###  svn-ignore
	Set "ignore" properties of subversion repo.

###  svn-commit-notify
	Notify hordes of svn commits, called from post-commit script in dir hooks/

##  Standalone C Programs

### stale-mtime.c
	Is a file "stale" based upon recent modifications?

###  idiff.c
	Simple interactive diff (based on rob & bwk code)

###  istext.c
	Is a file ascii text?

###  halloc.c
	Free a parent and you free the kids, a'la malloc().

###  fork-me.c
	Measure sequential fork/exec of a program

### Note:
	Create script is-dir-empty
