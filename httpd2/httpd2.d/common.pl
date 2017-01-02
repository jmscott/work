#
#  Synopis:
#	Common public routines used by httpd2 and users.
#
our (
	%CGI,
	%QUERY_ARG,
	%POST_VAR,
);
our ($left_RE) = '(?:\A|[&?])';
our ($right_RE) = '(?:=([^&?]*))?(?:&|\Z)';

END
{
	select((select(STDOUT), $| = 1)[0]);	    	# flush STDOUT
}

#
#  Synopsis:
#	die, gracefully logging to STDERR and possibly the client.
#  Description:
#	__DIE__ logs the error to STDERR and does a trivial effort to
#	comminicate the error to the client.
#
#	If the first part of the message matches ^([45]\d\d)\s*,
#	then the matching http status code is extracted and sent
#	to the client instead of a 500 error.
#  Note:
#	If the mime headers have already been written, which is
#	pretty common,  then, nothing is sent to the client,
#	which is bogus.  Surely needs improvement.  How about
#	content-type specific handlers invoked from an eval?
#  Usage:
#	die "good bye, cruel world";
#	die "404", "not sure what you mean";
#	die "404 not sure what you mean";
#
$SIG{__DIE__} = sub
{
	$CGI{dieing} = 1;		# process is dieing

	my $error_text = join('', @_);
	print STDERR "$CGI{process}: ", $error_text, "\n";

	my $status = 500;
	#
	#  The http status code can be specified as the first three digits
	#  of the message, like:
	#
	#	die "404", "what the hey are you doing!";
	#
	if ($error_text =~ /^([45]\d\d)\s*(.*)$/i) {
		$status = $1;
		$error_text = $2;
	}
	print STDERR 'status: ', $status, "\n";
	print STDERR $error_text, "\n";
	#
	#  If nothing has been written to STDOUT, then
	#  write the mime headers and indicate the error.
	#
	#  Also, I (jmscott) am not positive if tell(STDOUT) correctly
	#  indicates if STDOUT has actually been written to or not.
	#  Need to investigate.  Maybe we just need to know if the
	#  mime headers have been written but not any content, in which
	#  case we can test if tell(STOUT) == $sizeof_mime_headers.
	#
	my $wd = $CGI{work_dir};
	if (tell(STDOUT) == 0) {
		print <<END;
Status: $status\r
Content-Type: text/plain;  charset=iso-8859-1\r

$error_text
END
		#
		#  What is the following weirdness, you ask?
		#  Well, turns out that MS exploder won't always
		#  show the text of an error if
		#
		#	1)  running in "friendly" mode
		#	2)  length(text) < 512.
		#
		#  What the user sees is something like "Page Not Found",
		#  instead of a description from the server.
		#  Soo, we pad $error_text with space on the end when
		#  chatting with Explorer.
		#
		if (($status <= 400 && $status < 600) &&
		     length($error_text) < 512 &&
		    $ENV{HTTP_USER_AGENT} =~ /\WMSIE\W/) {
			print sprintf('% ' . (512 - length($error_text)) .'s');
		}
		my $port = ($ENV{SERVER_PORT} eq '80' ? '' : 
							":$ENV{SERVER_PORT}");
		print "\n\nFor help, see " .
			"http://$ENV{SERVER_NAME}$port$ENV{SCRIPT_NAME}?help\n";
		#
		#  Broadcast the location of the temporary work directory
		#  to aid debugging.
		#
		print "\n\nWork Directory: $wd" if $wd;
	} 
	print STDERR "See Work Directory: $wd" if $wd;
	exit 1;
};

#
#  Decode %HEX triplets in a url query argument.
#
sub decode_url_query_arg
{
	my $u = $_[0];

	#
	#  When to do the + versus %20 is still, after 20 years,
	#  not clear to me.  Certain browsers seem to gratuitously
	#  escape space as +.  Not sure why.  Perhaps and encoding
	#  attribute in the form.
	#
	$u =~ s/\+/ /g;

	#
	#  Escape % hex characters.  For example, %2B => +
	#
	$u =~ s/%([0-9A-Fa-f]{2})/chr(hex($1))/eg;
	return $u;
}

#
#  Encode "unwise" characters, space, quote, and apostrophe in a string 
#  intended to be the value of a query argument;  i.e.,
#
#	name=value
#
#  The definition of an unwise character is from RFC2732.
#
sub encode_url_query_arg
{
	my $u = $_[0];

	$u =~ s/([;\/?:@&=+ '"[\]])/sprintf("%%%02X",ord($1))/eg;
	return $u;
}

sub encode_html_entities
{
	my $h = $_[0];

	$h =~ s/&/\&#38;/gms;
	$h =~ s/</\&#60;/gms;
	$h =~ s/>/\&#62;/gms;
	$h =~ s/"/\&#34;/gms;
	$h =~ s/'/\&#39;/gms;

	return $h;
}

sub zap_uri_query_arg
{
	my $uri = shift @_;
	for (@_) {
		#
		#  Unfortunatly, zero-width assertions of \A (^) and \Z
		#  fail in a character class, so add extra substitutions.
		#
		$uri =~ s/[&?]${_}=[^&?]*//g;
		$uri =~ s/^${_}=[^&?]*//g;
	}
	$uri =~ s/^[&?]//;
	return $uri;
}

sub add_uri_query_arg
{
	my $uri = shift @_;
	if ($uri =~ /[&]/) {
		$uri .= '&amp;' . $_[0] . '=' . encode_url_query_arg($_[1]);
	} else {
		$uri .= '?' . $_[0] . '=' . encode_url_query_arg($_[1]);
	}
	return $uri;
}

#
#  Deprecated
#
sub decode_url_escape
{
        my($u) = @_;

        $u =~ s/\+/ /g;
        $u =~ s/%([0-9A-Fa-f]{2})/chr(hex($1))/eg;
        return $u;
}

1;
