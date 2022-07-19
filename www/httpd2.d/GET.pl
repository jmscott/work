#
#  Synopsis:
#	Implement the GET method executed from a cgi script.
#

our (
	%CGI,
	%QUERY_ARG,
	$left_RE,
	$right_RE,
);

our $PGDUMP_re = qr/^(STDOUT|STDERR|1|on|yes|y|t|true)$/i;

#
#  Synopsis:
#	Set values in %QUERY_ARG based on default attribute <query-args> twig.
#
sub set_QUERY_ARG_default
{
	my %arg = @_;

	my $query_args = $arg{root}->{'query-args'};
	return unless defined $query_args;
	for my $d (keys %{$query_args->{arg}}) {
		$QUERY_ARG{$d} = $query_args->{arg}->{$d}->{default};
	}
}

#
#  Synopsis:
#	Verify values in %QUERY_ARG agree with rules in <query-args>. 
#
sub set_QUERY_ARG
{
	my %arg = @_;

	my $query_args = $arg{root}->{'query-args'};
	return unless $query_args;

	my $me = "GET: $CGI{name}?out=$QUERY_ARG{out}";

	#
	#  For each query argument in the xml definition.
	#
	for my $a (keys %{$query_args->{arg}}) {
		my $required = $query_args->{arg}->{$a}->{required};
		#
		#  Not in the QUERY_STRING, so first check to see if required
		#  in /cgi/GET defaults.
		#
		unless ($ENV{QUERY_STRING} =~ /${left_RE}$a$right_RE/) {
			next if $required eq 'no';
			die "$me: missing required query argument: $a"
						if $required eq 'yes';
		}
		my $v = $1;
		#
		#  An empty but defined query arg is same as not satifying
		#  required.  In other words, we're flakey about nulls.
		#
		unless (length($v) > 0) {
			die "$me: empty required query argument: $a"
						       if $required eq 'yes';
			next;
		}
		#
		#  perl5_re must be defined.  why?
		#
		my $re = $query_args->{arg}->{$a}->{perl5_re};
		next unless $re;

		#
		#  When to do the + versus %20 is still, after 20 years,
		#  not clear to me.  Certain browsers seem to gratuitously
		#  escape space as +.  Not sure why.  Perhaps and encoding
		#  attribute in the form.
		#
		$v = decode_url_query_arg($v);
		die "$me: query arg $a=$v doesn't match perl5_re /^$re\$/"
						unless $v =~ m/^(?:${re})$/s;
		$QUERY_ARG{$a} = $v;
	}
}
my $content_type;

#
#  Process the url query arguments unless the cgi-script
#  was invoked as /cgi-bin/script?help.
#
unless ($ENV{QUERY_STRING} =~ /${left_RE}help$right_RE/) {
	#
	#  The query args precedence is wrong.
	#  The deepest definition should have highest precedence.
	#  
	#	1.	/cgi/GET/out/putter/query-args
	#	2.	/cgi/GET/out/query-args
	#	3.	/cgi/query-args[@domain]
	#
	#  The current code is backwards.
	#

	#
	#  Set the /cgi/GET/out Query Arguments.
	#  These query arguments apply to all <out><putter> elements in
	#  the cgi/xml specification.
	#
	&set_QUERY_ARG_default(
		root	=>	$CGI{GET}->{out}
	);

	#
	#  Apply the /cgi/GET/out level rules to parse, set and verify
	#  $QUERY_STRING arguments.
	#
	&set_QUERY_ARG(
		root	=>	$CGI{GET}->{out},
	);

	#
	#  Find the out/putter.
	#
	my $putter = $CGI{GET}->{out}->{putter}->{$QUERY_ARG{out}};

	#
	#  Verify the out putter has been defined.
	#
	die "unknown out putter script: $QUERY_ARG{out}" unless $putter;

	#
	#  Set the query arguments specific to a particular putter.
	#
	&set_QUERY_ARG_default(
		root	=>	$putter,
	);

	#
	#  Apply the /cgi/GET/out/putter level rules to parse, set and verify
	#  $QUERY_STRING arguments.
	#
	&set_QUERY_ARG(
		root	=>	$putter,
	);
	$content_type = $putter->{$QUERY_ARG{out}}->{'content-type'};
	#
	#  Set the content-type, walking up /cgi/GET/out/putter branch.
	#
	$content_type = $CGI{GET}->{out}->{putter}->
			{$QUERY_ARG{out}}->{'content-type'}
							unless $content_type;
	$content_type = $CGI{GET}->{out}->{'content-type'} unless $content_type;
} else {
	$QUERY_ARG{out} = 'help';

	#
	#  Set id and id_att variables.
	#
	$QUERY_ARG{id} = $1 if $ENV{QUERY_STRING} =~ /${left_RE}id$right_RE/;
	$QUERY_ARG{class} = $1
			if $ENV{QUERY_STRING} =~ /${left_RE}class$right_RE/;
	$content_type = 'text/html';
}

#
#  Send status header.
#
print <<END;
Status: 200
END

#
#  If content type is defined, then send to client and end headers.
#  Need a better way to disable closing the headers than content type.
#
print <<END if $content_type;
Content-Type: $content_type
\r
END

#
#  We build the id_att and id_class as convience to the scripts
#  that wish to add "id" and "class" attributes to the xml elements
#  they generate.  This allows perl code in the putter like:
#
#	print <<END;
#	<div$QUERY_ARG{id_att}$QUERY_ARG{class_att}>
#	END
#
$QUERY_ARG{id_att} = " id=\"$QUERY_ARG{id}\"" if defined $QUERY_ARG{id};
$QUERY_ARG{class_att} = " class=\"$QUERY_ARG{class}\""
						if defined $QUERY_ARG{class};

#  move the query arg trigger for an sql dump (locally) to $ENV{PGDUMP}.

if ($ENV{QUERY_STRING} =~ /${left_RE}PGDUMP$right_RE/) {
	my $v = $1;
	die "query arg: PGDUMP: unexpected value: $v" unless $v =~ $PGDUMP_re;
	print STDERR "query arg: PGDUMP=$v\n";
	$ENV{PGDUMP} = $v;
}
return require "$CGI{name}.d/$QUERY_ARG{out}.pl";
