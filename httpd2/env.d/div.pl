#
#  Synopsis:
#	Generate an <div> list of process enviroment variables.
#  Usage:
#	/cgi-bin/env?out=dl&id=unix-process&class=error
#  See Also:
#	/cgi-bin/env?out=help
#  Depends:
#	cgi.pl
#  SVN Id:
#	$Id$
#

print <<END;
<div$QUERY_ARG{id_att}$QUERY_ARG{class_att}>
END

#
#  Check for existence
#
die "missing required query argument 'var' or 'arg'" if 
			!defined $QUERY_ARG{var} && !defined $QUERY_ARG{arg};

#
#  Put the value of either an environment variable, named in
#  the 'var' argument or put a query argument, named in the 'arg' query
#  variable.
#
if (my $var = $QUERY_ARG{var}) {
	if (defined $ENV{$var}) {
		print &encode_html_entities($ENV{$var});
	}
} elsif (my $arg = $QUERY_ARG{arg}) {
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
	if ($ENV{QUERY_STRING} =~ /(?:\A|[&?])$arg(?:=([^&?]*))?(?:&|\Z)/) {
		print &encode_html_entities(&decode_url_query_arg($1));
	}
}
print <<END;
</div>
END

return 1;
