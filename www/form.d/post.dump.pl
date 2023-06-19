#
#  Synopsis:
#	Generate an html <div> element that is the dump of a http POST.
#  Usage:
#	<form action="/cgi-bin/env?out=post.dump" ...>
#  Description:
#	Generate a <div> html element that dumps the contents of an http 
#	POST command.  The output is suitable for debugging forms and not
#	good for much else.
#  HTML Outline:
#	<div [id=..] [class=..]>
#	  <h1>..
#	  <div class="form">
#	    <h2>
#	    <dl> 
#	  <div class="env">
#	    <h2>
#	    <dl>
#
#  Subversion Id:
#	$Id$
#
our (
	%QUERY_ARG,
	%POST_VAR
);

print <<END;
Content-Type: text/html

<div$QUERY_ARG{id_att}$QUERY_ARG{class_att}>
<h1>Output of <code>$ENV{SCRIPT_NAME}</code></h1>
END
print <<END;
 <div class="form">
  <h2>Posted Variables and Values</h2>
  <dl>
END

for (sort keys %POST_VAR) {
	my $dt_text = &encode_html_entities($_);
	my $dd_text;
	#
	#  Assemble the <dd> element.
	#  Multiply selected values are separated by tabs,
	#  so convert them to an <ul>.
	#
	if (index($POST_VAR{$_}, "\t") > -1) {
		$dd_text = "<ul>\n";
		#
		#  Write tab separated entities as an unordered list.
		#
		for my $m (split("\t", $POST_VAR{$_})) {
			#
			#  If string contains a newline the write as a pre.
			#
			if (index($m, "\n") > -1) {
				$dd_text .= sprintf("<li><pre>%s</pre></li>\n",
						&encode_html_entities($m));
			} else {
				$dd_text .= sprintf("<li>%s</li>\n",
						&encode_html_entities($m));
			}
		}
		$dd_text .= "</ul>\n";
	} else {
		#
		#  If string contains a newline, then write as a pre.
		#  Probably out to just do a byte dump.
		#
		if (index($POST_VAR{$_}, "\n") > -1) {
			$dd_text = '<pre>'.&encode_html_entities($POST_VAR{$_}).
				   '</pre>';
		} else {
			$dd_text = &encode_html_entities($POST_VAR{$_});
		}
	}
	print <<END;
   <dt>$dt_text</dt>
   <dd>$dd_text</dd>
END
}
print <<END;
  </dl>
 </div>
END

print <<END;
 <div class="env">
  <h2>Process Environment</h2>
  <dl>
END

my ($dt_text, $dd_text);
for (sort keys %ENV) {
	$dt_text = &encode_html_entities($_);
	$dd_text = &encode_html_entities($ENV{$_});
	print <<END;
   <dt>$dt_text</dt>
   <dd>$dd_text</dd>
END
}
print <<END;
  </dl>
 </div>
END


print "</div>\n";
