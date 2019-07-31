#
#  Synopsis:
#	Generate <dl> list of process environment variables.
#  Usage:
#	/cgi-bin/env?out=dl&id=unix-process&class=stunning
#  See Also:
#	/cgi-bin/env?out=help
#  Depends:
#	&encode_html_entities()
#  SVN Id:
#	$Id$
#

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
	my $dd_text = &encode_html_entities($ENV{$_});
	print <<END;
 <dt><span>$dt_text</span></dt>
 <dd><span>$dd_text</span></dd>
END
}

#
#  Close </dt>
#
print <<END;
</dl>
END
