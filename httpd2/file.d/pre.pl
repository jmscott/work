#
#  Synopsis:
#	Write the contents of a text file as a <pre>
#  Usage:
#	/cgi-bin/file?out=pre&path=lib/file.d/pre.pl
#  Depends:
#	&encode_html_entities($buf);
#
our %QUERY_ARG;

my $path = "$ENV{SERVER_ROOT}/$QUERY_ARG{path}";

open(FILE_PRE, "<$path") or die "open(<$path) failed: $!";

#
#  Open <pre>.
#  Notice the leading newline in the <<END documents
#  is clipped using substr().
#
print substr(<<END, 0, -1);
<pre$QUERY_ARG{id_att}$QUERY_ARG{class_att}>
END

my ($buf, $nread);

#
#  Read the file.
#  Note: need to encode unprintable characters.
#
while (($nread = sysread(FILE_PRE, $buf, 4096)) > 0) {
	print &encode_html_entities($buf);
}
die "sysread($path) failed: $!" if !defined $nread;
close(FILE_PRE) or die "close($path) failed: $!";

#
#  Close <pre>
#
print "</pre>\n";

1;
