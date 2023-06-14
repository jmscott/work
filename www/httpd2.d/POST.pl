#
#  Synopsis:
#	Handle a POST http method, invoking code based on %CGI contents.
#  Note:
#	Need to added support for PGDUMP variable.
#
#	Multipart mime is not supported, which is wrong.
#	Need to investigate
#
#		https://metacpan.org/release/MIME-tools
#

our (
	%CGI,
	%POST_VAR,
	$POST_DATA,
);

binmode STDIN;

#  should we parse the body of the POST or passthough to the script.


my $CL = $ENV{CONTENT_LENGTH};
my $CT = $ENV{CONTENT_TYPE};

my $MAX_CONTENT_LENGTH = 100 * 1024 * 1024;	#  100meg

#  Verify the absolute maximum length, regardless of encoding
#  Specifc encodings may enforce stricter limits.

die "Content-Length too big: $CL > $MAX_CONTENT_LENGTH"
				if $CL > $MAX_CONTENT_LENGTH
;
if ($CL > 0) {
	#
	#  Insure the content is not too big.
	#
	die "CONTENT_LENGTH too large: " .  "$ENV{CL} > $MAX_CONTENT_LENGTH"
						if $CL > $MAX_CONTENT_LENGTH
	;
	#
	#  Read the posted content from stdin.
	#  Ought to timeout read() call.
	#
	my ($buf, $nread);
	while ($CL > 0 && ($nread = read(STDIN, $buf, $CL)) > 0) {
		$CL -= $nread;
		$POST_DATA .= $buf;
	}
	die "POST: read($CL) failed: $!" if $nread <= 0;
} else {
	die "http: POST: content length not > 0: length=$CL";
}

#
#  Call the appropriate parser to map build the POST_* variables.
#


if ($CT eq 'application/x-www-form-urlencoded') {
	require 'httpd2.d/www-form-urlencoded.pl';
} elsif ($CT =~ m/application\/multipart-form-data/ or
         $CT =~ m/multipart\/form-data/) {
	require 'httpd2.d/multipart-form-data.pl';
} else {
	die "XPOST: unknown CONTENT_TYPE: $CT";
}

#
#  Check for any missing POST values.
#
#  Why is "in" required.  If not defined, why not default to the
#  name of the form?
#
for (keys %{$CGI{POST}->{in}->{var}}) {
	my $required = $CGI{POST}->{in}->{var}->{$_}->{required};
	next unless $required eq 'yes';
	die "variable $_ is required in POST" if !$POST_VAR{$_};
}

#
#  Synopsis:
#	Verify values in %POST_VAR agree with rules in <vars>
#
sub validate_POST_VAR
{
	my %arg = @_;

	my $vars = $arg{vars};
	return unless $vars;

	my $me = "POST: $CGI{name}?in=$POST_VAR{in}";

	#
	#  For each query argument in the xml definition.
	#
	for my $a (keys %{$vars->{var}}) {
		#
		#  Not in the QUERY_STRING, so first check to see if required
		#  in /cgi/GET defaults.
		#
		my $v;
		unless ($v = $POST_VAR{$a}) {
			die "$me: missing required variable: $a"
			if $vars->{var}->{$a}->{required} eq 'yes';
			next;
		}
		my $re = $vars->{var}->{$a}->{perl5_re};
		next unless $re;
		die "$me: var($a) value($v) doesn't match perl5_re($re)"
						unless $v =~ /^${re}$/s;
	}
}

my $in = $POST_VAR{in};
die "POST: variable 'in' not defined" unless $in;

#
#  Verify that the script referenced by 'in' variable
#  has been defined in the cgi/xml file.
#
die "/cgi/POST/in/putter[name=$in] not defined in $CGI{name}.cgi"
			unless $CGI{POST}->{in}->{putter}->{$in}->{name};

#
#  Create id_att/class_att for any id/class variables passed into form.
#
$POST_VAR{id_att} = " id=\"$POST_VAR{id}\"" if defined $POST_VAR{id};
$POST_VAR{class_att} =" class=\"$POST_VAR{class}\"" if defined $POST_VAR{class};

&validate_POST_VAR(
	vars	=>	$CGI{POST}->{in}->{putter}->{$in}->{vars},
);

#
#  Require the script to do the real work.
#
return require "$CGI{name}.d/$in.pl";
