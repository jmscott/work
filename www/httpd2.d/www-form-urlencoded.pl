#
#  Synopsis:
#	Parse contents of $POST_DATA into %POST_VAR
#

our(
	$POST_DATA,
	%POST_VAR,
);

#
#  For each name-value pair:
#
foreach my $pair (split(/&/, $POST_DATA)) {
	#
	#  Split the pair up into individual variables.
	#
	my ($name, $value) = split(/=/, $pair, 2);

	#
	#  Decode the form encoding on the name and value variables.
	#
	$name =~ tr/+/ /;
	$name =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;

	#
	#  Remove null bytes
	#
	$name =~ tr/\0//d;

	#
	#  Convert %XX chars to characters.
	#
	$value =~ tr/+/ /;
	$value =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
	$value =~ tr/\0//d;

	#
	#  Replace tabs with white space.
	#  Be sure to document this behavior in cgi2perl5-help.xsl.
	#  Probably ought to be an attribute in xml cgi file.
	#
	$value =~ s/\t+/ /g;		# replace tabs with white space
	$value =~ s/^\s+|\s+$//;	# strip leading/trailing white

	#
	#  Separate multiple values with tabs.
	#
	#  Probably ought to make the separator character settable
	#  from the xml spec file.  Also need to verify that the variable
	#  is intended to have mutiple values ... not all are.
	#
	$POST_VAR{$name} .= "\t" if $POST_VAR{$name};
	$POST_VAR{$name} .= $value;
}

1;
