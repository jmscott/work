#
#  Synopsis:
#	Generate <input> element from value of url query argument.
#  Usage:
#	/cgi-bin/env?out=input&type=text&arg=PGDATABASE&ro=yes
#  See Also:
#	/cgi-bin/env?out=help
#  Depends:
#	&encode_html_entities()
#
our (%QUERY_ARG, $right_RE, $left_RE);

#
#  Derive the name attribute from either an explicit name query arg
#  or derive from the id, then query arg field.
#
my $name_att;
my $name = ($QUERY_ARG{name} or $QUERY_ARG{id} or
			($QUERY_ARG{qarg} or $QUERY_ARG{evar}));
$name_att = sprintf(' name="%s"', &encode_html_entities($name)) if $name;

#
#  Derive name="value and type="value" attributes.
#
my $type_att = sprintf(' type="%s"', &encode_html_entities($QUERY_ARG{type}));

#
#  The value of the query arg
#
my $value_att;
if (my $qarg = $QUERY_ARG{qarg}) {
	if ($ENV{QUERY_STRING} =~ /${left_RE}$qarg$right_RE/) {
		my $v = &encode_html_entities($1);
		$value_att = sprintf(' value="%s"', $v);
	}
} elsif (my $evar = $QUERY_ARG{evar}) {
	if ($ENV{QUERY_STRING} =~ /${left_RE}$evar$right_RE/) {
		my $v = &encode_html_entities(ENV{$1});
		$value_att = sprintf(' value="%s"', $v);
	}
} else {
	die "either 'qarg' or 'evar' must be a query argument";
}

#
#  Set the field to readonly if requested or to readonly if the write once
#  field is set and the value is defined.
#
#  Probably ought to add the class 'readonly' to the class list when 'wo'
#  set the readonly attribute.
#
my $readonly_att;
$readonly_att = ' readonly="readonly"'
	if ($QUERY_ARG{inro} eq 'yes' ||
	    ($QUERY_ARG{inro} eq 'wo' && $value_att));

#
#  Put <input />
#
print <<END;
<input$QUERY_ARG{id_att}$QUERY_ARG{class_att}
  $name_att$type_att$value_att$readonly_att
/>
END
