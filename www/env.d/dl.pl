#
#  Synopsis:
#	Generate <dl> list of process environment variables.
#  Usage:
#	/cgi-bin/env?out=dl&id=unix-process&class=stunning
#  See Also:
#	/cgi-bin/env?out=help
#  Depends:
#	&encode_html_entities()
#

our %QUERY_ARG;

#
#  Open <dl>
#
print <<END;
<dl$QUERY_ARG{id_att}$QUERY_ARG{class_att}>
END

#
#  <dt><dd> for each entry in %ENV.
#
for (sort keys %ENV) {
	my $dt_text = &encode_html_entities($_);
	my ($dd_text, $dd_span_class);
	if ($ENV{$_}) {
		$dd_text = &encode_html_entities($ENV{$_});
	} else {
		$dd_text = '&#10240;';
		$dd_span_class = ' class="empty"';
	}
	print <<END;
 <dt><span>$dt_text</span></dt>
 <dd><span$dd_span_class>$dd_text</span></dd>
END
}

#
#  Close </dt>
#
print <<END;
</dl>
END
