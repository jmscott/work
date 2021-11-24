#
#  Synopsis:
#	Trivial front end functions to PostgreSQL DBD::Pg functions.
#  Return:
#	Always implies success, die otherwise
#  Note:
#	CACHED is broken.  Zap it!
#
#	existence of "tag" not tested in dbi_pg_connect!.
#
#	Added example of query called from cgi-bin, similar to env.cgi and
#	form.cgi.
#
#	On macports, perl -W generates complaints
#
#		Subroutine version::declare redefined at ...
#

#
#  Manually set LD_LIBRARY_PATH.  Redhat 6.2 apache has problem setting
#  LD_LIBRARY_PATH.  Not sure why.
#

$ENV{LD_LIBRARY_PATH} = "$ENV{PGHOME}:$ENV{LD_LIBRARY_PATH}" if $ENV{PGHOME};

use DBD::Pg;

our %QUERY_ARG;

my ($CACHED, @OPEN);

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
		#  Should Scalar:;Util be used instead of == ?
		#
		$CACHED = undef if $db == $CACHED;

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

	return $CACHED if $CACHED;

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
	return $CACHED = $db;
}

#
#  Note:
#	>$tag.sql silently fails!
#
sub dbi_pg_dump
{
	my %arg = @_;
	my (
		$sql,
		$tag,
		$pgdump,
	) = (
		$arg{sql},
		$arg{tag},
		$arg{pgdump} ? $arg{pgdump} : $ENV{pgdump},
	);

	print STDERR "dbi_pg_dump: $tag: requested, pgdump=$pgdump\n";

	if ($pgdump eq 'STDERR') {
		print STDERR $tag, ': sql: ', $sql, "\n";
	} elsif ($pgdump eq 'STDOUT') {
		print STDOUT $tag, ': sql: ', $sql, "\n";
	} elsif ($pgdump eq '1' || $pgdump eq 'yes') {

		my ($TMPDIR, $sql_path) = ($ENV{TMPDIR});

		$TMPDIR = '/tmp' unless $TMPDIR;
		$sql_path = ">$TMPDIR/$tag.sql";
		print STDERR "dbi_pg_dump: sql path: $sql_path\n";

		my $T;
		open($T, $sql_path) or die "$tag: open($sql_path) failed: $!";
		print $T $sql, ";\n";
		close $T or die "close($sql_path) failed: $!";
	} elsif ($pgdump ne '0' && $pgdump ne 'off') {
		die "dbi_pg_dump: unknown dump arg: $pgdump";
	}
}

#
#  Synopsis:
#	Prepare&execute an sql select query and return readable results handle.
#  Arguments:
#	All are required, including empty argv[].
#
#	db	=>	DBI handle opened with dbi_pg_connect()
#	tag	=>	short tag describing query (required)
#	argv	=>	query arguments: argv => [$1, $2, ...]
#	sql	=>	sql to execute
#	pgdump	=>	STDERR|STDOUT|1|yes
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

	dbi_pg_dump(%arg) if $QUERY_ARG{pgdump} || $arg{pgdump};

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
#	All are required, including empty argv[].
#
#	db	=>	DBI handle opened with dbi_pg_connect()
#	tag	=>	short tag describing query (required)
#	argv	=>	query arguments: argv => [$1, $2, ...]
#	sql	=>	sql to execute
#  Returns:
#	DBI->prepare() query handle, after query has been executed.
#
sub dbi_pg_exec
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

	die 'dbi_pg_exec: missing variable: tag' unless $tag;
	die 'dbi_pg_exec: missing variable: db' unless $db;
	die 'dbi_pg_exec: missing variable: sql' unless $sql;
	die 'dbi_pg_exec: missing variable: argv' unless $argv;

	dbi_pg_dump(%arg) if $QUERY_ARG{pgdump};

	my $q = $db->prepare($sql) or die
			'dbi_pg_exec: prepare($tag) failed: ' . $db->errstr;
	$q->execute(@{$argv}) or die "dbi_pg_exec: ($tag) failed: ". $q->errstr;
	return $q;
}

1;
