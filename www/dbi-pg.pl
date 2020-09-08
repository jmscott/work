#
#  Synopsis:
#	Trivial front end functions to PostgreSQL DBD::Pg functions.
#  Return:
#	Always implies success, die otherwise
#  Note:
#	Added example of query called from cgi-bin, similar to env.cgi and
#	form.cgi.
#
#	perl -W generates complaints
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
		$db->disconnect() or print STDERR "DBI->disconnect failed: ".
							DBI->errstr;
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

	my (
		$tag,
		$uri,
	) = (
		$arg{tag},
	);

	return $CACHED if $CACHED;

	if ($ENV{PGHOST}) {
		$uri = sprintf('dbi:Pg:host=%s;port=%s;dbname=%s;user=%s',
				$ENV{PGHOST},
				$ENV{PGPORT},
				$ENV{PGDATABASE},
				$ENV{PGUSER}
			);
	} else {
		#
		#  Use unix pipe??
		#
		$uri =
	              'dbi:Pg:host=/tmp;port=5432;dbname=setspace;user=setspace'
		;
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
		$QUERY_ARG{pgdump},
	);

	print STDERR "dbi_pg_dump: $tag: requested\n";

	if ($pgdump eq 'STDERR') {
		print STDERR $tag, ': sql: ', $sql, "\n";
	} elsif ($pgdump eq 'STDOUT') {
		print STDOUT $tag, ': sql: ', $sql, "\n";
	} elsif ($pgdump eq '1' || $pgdump eq 'yes') {

		my ($TMPDIR, $sql_path) = ($ENV{TMPDIR});

		$TMPDIR = '/tmp' unless $TMPDIR;
		$sql_path = ">$TMPDIR/$tag.sql";

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

	dbi_pg_dump(%arg) if $QUERY_ARG{pgdump};

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

1;
