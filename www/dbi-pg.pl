#
#  Synopsis:
#	Trivial front end functions to PostgreSQL DBD::Pg functions.
#  Return:
#	Always implies success, die otherwise
#  Note:
#	why is 'httpd2.d/common.pl' required?
#
#	Carefully examine the PGDUMP=<tag> option.  I (jmscott) consistent;y
#	see failed SQL dumps for particular queries.  May be realated to not
#	testing for "tag" in dbi_pg_connect!
#
#	Consider added option PGDUMP=help
#
#	Rewrite require statements to reference jmscott/httpd2.d/...'
#	This confines env var PERL5 to $SERVER_ROOT/lib.
#
#	existence of "tag" not tested in dbi_pg_connect!
#
#	Added example of query called from cgi-bin, similar to env.cgi and
#	form.cgi.
#

require 'httpd2.d/common.pl';

#
#  Manually set LD_LIBRARY_PATH.  Redhat 6.2 apache has problem setting
#  LD_LIBRARY_PATH.  Not sure why.
#

our %QUERY_ARG;
our ($left_RE, $right_RE);

our $PGDUMP_re = qr/^(STDOUT|STDERR|1|0|(?:[a-zA-Z][a-zA-Z_-]{0,62}))$/i;

if ($ENV{QUERY_STRING} =~ /${left_RE}PGDUMP$right_RE/) {
	my $v = $1;
	die "query arg: PGDUMP: unexpected value: $v" unless $v =~ $PGDUMP_re;
	print STDERR "query arg: PGDUMP=$v\n";
	$ENV{PGDUMP} = $v;
}

$ENV{LD_LIBRARY_PATH} = "$ENV{PGHOME}:$ENV{LD_LIBRARY_PATH}" if $ENV{PGHOME};

use DBD::Pg;

my @OPEN;

END
{
	#
	#  Shutdown the DBI connections.
	#
	for my $db (@OPEN) {
		next unless $db;

		#  Note: no error logging, since dbu grumbles about unrolled
		#        statements.
		$db->disconnect();
	}
}

sub dbi_pg_disconnect
{
	while (my $db = shift @_) {
		next unless $db;
		$db->disconnect() or
				warn "DBI->disconnect() failed: " . DBI->errstr;
		#
		#  Iterate backwards through the array of open dbs,
		#  splicing out open dbs.
		#
		for (my $i = $#OPEN;  $i >= 0;  --$i) {
			next unless $OPEN[$i] == $db;
			splice @OPEN, $i, 1;
		}
	}
}

sub dbi_pg_connect
{
	my %arg = @_;

	my $uri;
	if ($arg{PGHOST}) {
		$uri = sprintf('dbi:Pg:host=%s;port=%s;dbname=%s;user=%s',
				$arg{PGHOST},
				$arg{PGPORT},
				$arg{PGDATABASE},
				$arg{PGUSER}
			);
	} elsif ($ENV{PGHOST}) {
		$uri = sprintf('dbi:Pg:host=%s;port=%s;dbname=%s;user=%s',
				$ENV{PGHOST},
				$ENV{PGPORT},
				$ENV{PGDATABASE},
				$ENV{PGUSER}
			);
	} else {
		die 'DBI->connect: neither arg PGHOST nor ENV{PGHOST} exist';
	}

	my $db = DBI->connect($uri, '', '', {
                        AutoCommit      =>      1
	}) or die "DBI->connect($uri) failed: " . DBI->errstr;

	#
	#  Note:
	#	Only one open, so build stack of @OPEN?
	#
	push @OPEN, $db;
	return $db;
}

#
#  Dump an sql query to $TMPDIR/$tag.$$.sql
#
#  Note:
#	Need to dump $argv as sql comments!
#
sub dbi_PGDUMP
{
	my %arg = @_;
	my (
		$sql,
		$tag,
		$argv,
		$PGDUMP,
	) = (
		$arg{sql},
		$arg{tag},
		$arg{argv},

		$arg{PGDUMP} ? $arg{PGDUMP} : $ENV{PGDUMP},
	);

	return unless $PGDUMP;
	die "dbi_PGDUMP: unexpected value: $PGDUMP"
		unless $PGDUMP =~ $PGDUMP_re
	;

	print STDERR "dbi_PGDUMP: $tag: requested, PGDUMP=$PGDUMP\n";

	if ($PGDUMP eq 'STDERR') {
		print STDERR $tag, ': sql: ', $sql, "\n";
	} elsif ($PGDUMP eq 'STDOUT') {
		print STDOUT $tag, ': sql: ', $sql, "\n";
	} else {

		my ($TMPDIR, $sql_path) = ($ENV{TMPDIR});

		#  "tag" may be "1" to dump all queries or "tag" may be the
		#  name of a specific query to dump.

		if ($PGDUMP =~ m/^[a-z]/i && $PGDUMP ne $tag) {
			print STDERR
				'dbi_PGDUMP: off (ok): ',
				"$tag != $PGDUMP\n";
			return;
		}
		print STDERR "dbi_PGDUMP: on: $tag\n";

		$TMPDIR = '/tmp' unless $TMPDIR;
		$sql_path = ">$TMPDIR/$tag.$$.sql";
		print STDERR "dbi_PGDUMP: sql temp path: $sql_path\n";

		my $T;
		open($T, $sql_path) or die "$tag: open($sql_path) failed: $!";

		print $T <<END;
/*
 *  Tag:
 *	$tag
 *  PGDUMP:
 *	$PGDUMP
 *  ArgV:
END
		#  print the args in argv in a comment header
		print $T " *	$_\n" foreach (@$argv);
		print $T " */\n";

		print $T $sql, ";\n";
		close $T or die "close($sql_path) failed: $!";
	}
}

#
#  Synopsis:
#	Prepare&execute an sql select query and return readable results handle.
#  Arguments:
#	All are required, including empty argv[], expect PGDUMP.
#
#	db	=>	DBI handle opened with dbi_pg_connect()
#	tag	=>	short tag describing query (required)
#	argv	=>	query arguments: argv => [$1, $2, ...]
#	sql	=>	sql to execute
#	PGDUMP	=>	/^(STDERR|STDOUT|1|t|true|y|yes|on)/i
#  Returns:
#	DBI->prepare() query handle, after query has been executed.
#
sub dbi_pg_select
{
	my %arg = @_;

	my (
		$db,
		$sql,
		$argv,
		$tag,
	) = (
		$arg{db},
		$arg{sql},
		$arg{argv},
		$arg{tag},
	);

	die 'dbi_pg_select: missing variable: tag' unless $tag;
	die 'dbi_pg_select: missing variable: db' unless $db;
	die 'dbi_pg_select: missing variable: sql' unless $sql;
	die 'dbi_pg_select: missing variable: argv' unless $argv;

	dbi_PGDUMP(%arg);

	my $q = $db->prepare($sql) or die
			'dbi_pg_select: prepare($tag) failed: ' . $db->errstr;
	$q->execute(@{$argv}) or
			die "dbi_pg_select: ($tag) failed: ". $q->errstr;
	#
	#  Insure caller passed a true select statement.
	#
	$q->{NUM_OF_FIELDS} > 0 or
			die 'dbi_pg_select: expected NUM_OF_FIELDS > ';
	return $q;
}

#
#  Synopsis:
#	Prepare&execute an sql query and return readable results handle.
#  Arguments:
#	All are required, including empty argv[], except PGDUMP.
#
#	db	=>	DBI handle opened with dbi_pg_connect()
#	tag	=>	short tag describing query (required)
#	argv	=>	query arguments: argv => [$1, $2, ...]
#	sql	=>	sql to execute
#	PGDUMP	=>	/^(STDERR|STDOUT|1|t|true|y|yes|on)/i
#  Returns:
#	DBI->prepare() query handle, after query has been executed.
#
sub dbi_pg_execute
{
	my %arg = @_;

	my (
		$db,
		$sql,
		$argv,
		$tag,
	) = (
		$arg{db},
		$arg{sql},
		$arg{argv},
		$arg{tag},
	);

	die 'dbi_pg_execute: missing variable: tag' unless $tag;
	die 'dbi_pg_execute: missing variable: db' unless $db;
	die 'dbi_pg_execute: missing variable: sql' unless $sql;
	die 'dbi_pg_execute: missing variable: argv' unless $argv;

	dbi_PGDUMP(%arg);

	my $q = $db->prepare($sql) or die
			'dbi_pg_execute: prepare($tag) failed: ' . $db->errstr;
	$q->execute(@{$argv}) or
		die "dbi_pg_execute: ($tag) failed: ". $q->errstr
	;
	return $q;
}

1;
