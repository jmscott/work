#
#  Synopsis:
#	Write the value of an environment variable or query argument as text.
#  Usage:
#	/cgi-bin/env?out=text&qarg=var
#  Depends:
#	&encode_html_entities(), %QUERY_ARG
#

our (%QUERY_ARG, $left_RE, $right_RE);

#
#  Put the value of either an environment variable, named in
#  the 'var' argument or put a query argument, named in the 'arg' query
#  variable.
#
if (my $evar = $QUERY_ARG{evar}) {
	if (defined $ENV{$evar}) {
		print &encode_html_entities($ENV{$evar});
	}
} elsif (my $qarg = $QUERY_ARG{qarg}) {
	#
	#  Since we don't know the argument being passed, we can't depend
	#  upon it being stored in $QUERY_ARG.  Therefore, parse out the
	#  query argument here. 
	#
	#  Shouldn't this be relegated to a subroutine.
	#
	#  Also, I (jmscott) am not convinced this is the correct algorithm for
	#  decoding/encoding the values in QUERY_STRING.  Need to
	#  investigate more thoroughly ...
	#
	if ($ENV{QUERY_STRING} =~ /${left_RE}$qarg$right_RE/) {
		print &encode_html_entities(&decode_url_query_arg($1));
	}
} else {
	die "either 'evar' or 'qarg' must be a query argument";
}

return 1;
