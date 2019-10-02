#
#  Synopsis:
#	Generate <input> element from using env var or a query arg.
#  Usage:
#	/cgi-bin/jmscott/env?out=input&type=text&arg=PGDATABASE&ro=yes
#  See Also:
#	/cgi-bin/jmscott/env?out=help
#  Depends:
#	&encode_html_entities()
#
our (
	%QUERY_ARG,
	$right_RE,
	$left_RE
);

my ($name, $src) = ($QUERY_ARG{name}, $QUERY_ARG{src});

#
#  Derive the name attribute from either an explicit name query arg
#  or derive from the id, then query arg field.
#
my $name_att;
my $iname = $QUERY_ARG{iname};
unless ($iname) {
	$iname = $QUERY_ARG{id};
	$iname = $name unless $iname;
}

#  Build the name="xxx" attribute.

$name_att = sprintf(' name="%s"', &encode_html_entities($iname));

#  Build the type="$QUERY_ARG{type}" attribute

my $type_att = sprintf(' type="%s"', &encode_html_entities($QUERY_ARG{type}));

#
#  Derive the value of the <input value=".. /> attribute.
#
my $value;
if ($src eq 'env') {
	$value = $ENV{$name};
} elsif ($ENV{QUERY_STRING} =~ /${left_RE}$name$right_RE/) {
	$value = &decode_url_query_arg($1);
}

my $value_att = sprintf(' value="%s"', encode_html_entities($value));

#
#  Set the field to readonly if requested or to readonly if the write once
#  field is set and the value is defined.
#
#  Probably ought to add the class 'readonly' to the class list when 'wo'
#  set the readonly attribute.
#
my $readonly_att;
$readonly_att = ' readonly="readonly"' if $QUERY_ARG{ro} eq 'yes';

#
#  Put <input />
#
print <<END;
<input$QUERY_ARG{id_att}$QUERY_ARG{class_att}
  $name_att$type_att$value_att$readonly_att
/>
END
