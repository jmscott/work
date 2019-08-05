#
#  Synopsis:
#	Trivial front end functions to PostgreSQL DBD::Pg functions.
#  Return:
#	Always implies success, die otherwise
#

#
#  Manually set LD_LIBRARY_PATH.  Redhat 6.2 apache has problem setting
#  LD_LIBRARY_PATH.  Not sure why.
#
$ENV{LD_LIBRARY_PATH} = "$ENV{PGHOME}:$ENV{LD_LIBRARY_PATH}" if $ENV{PGHOME};
use DBD::Pg;

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

sub dbi_disconnect
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

sub dbi_connect
{
	my %arg = @_;
	my $uri;

	my $trace = $arg{trace};

	DBI->trace($trace) if $trace;

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
	       'dbi:Pg:host=/tmp;port=5432;dbname=setspace;user=setspace';
	}

	my $db = DBI->connect($uri, '', '', {
                        AutoCommit      =>      1
	}) or die "DBI->connect($uri) failed: " . DBI->errstr;

	push @OPEN, $db;
	return $CACHED = $db;
}

sub dbi_dump
{
	my %arg = @_;
	my (
		$sql,
		$tag,
		$dump,
	) = (
		$arg{sql},
		$arg{tag},
		$arg{dump},
	);

	if ($dump eq 'STDERR') {
		print STDERR $tag, ': sql: ', $sql, "\n";
	} elsif ($dump eq 'STDOUT') {
		print STDOUT $tag, ': sql: ', $sql, "\n";
	} elsif ($dump =~ /^>>?/) {
		print STDERR "sql dump: $dump", "\n";
		my $now = `date`;  chomp $now;

		my $T;
		open($T, $dump) or die "open($dump) failed: $!";
		print $T $sql, ";\n";
		close $T or die "close($dump) failed: $!";
	} else {
		print STDERR "dbi_dump: unknown dump arg: $dump";
	}
}

#
#  Synopsis:
#	Prepare&execute an sql select query and return readable results handle.
#  Arguments:
#	db	=>	DBI handle opened with dbi_connect()
#	tag	=>	short tag describing query
#	dump	=>	dump sql to STDERR, STDOUT, >file, or >>file.
#	trace	=>	set DBI network trace flags: 0-15
#	sql	=>	sql to execute
#  Returns:
#	DBI->prepare() query handle, after query has been executed.
#
sub dbi_select
{
	my %arg = @_;

	my (
		$db,
		$sql,
		$tag,
		$dump,
		$trace,
		$q
	) = (
		$arg{db},
		$arg{sql},
		$arg{tag},
		$arg{dump},
		$arg{trace}
	);

	$db = dbi_connect(%arg) unless $db;
	DBI->trace($trace) if $trace;
	dbi_dump(%arg) if $dump;
	$q = $db->prepare($sql) or die
			'dbi_select: prepare($tag) failed: ' . $db->errstr;
	$q->execute() or die "dbi_select: ($tag) failed: ". $q->errstr;
	#
	#  Insure caller passed a true select statement.
	#
	$q->{NUM_OF_FIELDS} > 0 or
			die 'dbi_select: expected NUM_OF_FIELDS > ';
	return $q;
}

#
#  Synopsis:
#	Execute a single sql statement and return numbers of rows affected
#  Arguments:
#	db	=>	DBI handle opened with dbi_connect()
#	tag	=>	short tag describing query
#	dump	=>	dump to STDERR, STDOUT, >file, or >>file.
#	trace	=>	set DBI network trace flags: 0-15
#	sql	=>	sql to execute
#  Returns:
#	Number of rows affected.
#
sub dbi_do
{
	my %arg = @_;

	my (
		$db,
		$sql,
		$tag,
		$dump,
		$trace,
		$q
	) = (
		$arg{db},
		$arg{sql},
		$arg{tag},
		$arg{dump},
		$arg{trace}
	);

	die "dbi_do: missing 'tag' argument" unless $tag;

	DBI->trace($trace) if $trace;
	dbi_dump(%arg) if $dump;
	my $nrows;
	$nrows = $db->do($sql) or die
				"dbi_do: do($tag) failed: " . $db->errstr;
	return $nrows eq '0E0' ? 0 : $nrows;
}

1;
