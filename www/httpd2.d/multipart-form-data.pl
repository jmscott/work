#
#  Synopsis:
#	Parse multipart mime data in %POST_VAR and store files in temp dir.
#  Notes:
#	Fast and untested parser of multipart mime data.
#	Needs much tuning.
#
our(
	$POST_DATA,
	%POST_VAR,
	%POST_VAR_FILENAME
);

my $dump_parts = 1;

print <<END if $dump_parts;
Content-Type: text/plain

END

#
#  Parse the boundary from the content type.
#
my $boundary;
die "can't locate boundary in CONTENT_TYPE: $ENV{CONTENT_TYPE}"
	unless $ENV{CONTENT_TYPE} =~ m/\Wboundary=(-+.*)/;
$boundary = $1;
die "CONTENT_TYPE boundary seems too small: $boundary"
						unless length($boundary) > 8;
print <<END if $dump_parts;
Content-Type: $ENV{CONTENT_TYPE}
    Boundary: $boundary
 Begin Parts:
END

#
#  Loops through the parts.
#
#  Notice the trailing -- on the boundary pattern ... I'm not sure
#  if this is part of the standard ... need to investigate.
#
for my $part (split(/(\r\n)?--$boundary(?:--)?\r\n/, $POST_DATA)) {
	next if $part =~ /^[\s\n\r]*$/;

	my $length = length($part);
	my ($headers, $body, $cd, $name, $filename, $ct, $cl) =
						split('\r\n\r\n', $part,2)
	;
	$headers =~ s/^\s*|\s*$//g;

	#
	#  Extract content disposition and name.
	#
	die "can't extract content dispostion from mime headers: $headers"
	    unless $headers =~m/.*Content-Disposition: form-data;([^\r\n]+).*/i
	;
	$cd = $1;

	#
	#  Extract the name.
	#
	die "can't extract name from mime headers"
				unless $cd =~ m/(?:\W|\A)name="([^"]+)".*/i
	;
	$name = $1;
	#
	#  Extract the filename.
	#
	$POST_VAR_FILENAME{$name} = $1
				if $cd =~ m/(?:\W|\A)filename="([^"]+)".*/i
	;

	#
	#  Extract content type for binary data
	#
	if ($headers =~ m/.*Content-Type: ([^\r\n]+).*/i) {
		$ct = $1;
	} else {
		$ct = '';
	}

	$POST_VAR{$name} = $body;

	if ($dump_parts) {
		my $body_a;
		if ($body =~ m/[\000-\010\016-\037\177-\377]/) {
			$body_a = '**NON ASCII DATA**'
		} else {
			$body_a = $body;
		}
		print <<END;

	 Begin Part:
             Length: $length

      Begin Headers:
$headers
        End Headers:

Content-Disposition: $cd
       Content-Type: $ct
	       Name: $name
	  File Name: $POST_VAR_FILENAME{$name}
               Body: $body_a

	   End Part:
END
	}
}

if ($dump_parts) {
	print <<END if $dump_parts;

   End Parts:
END
	exit;
}

1;
