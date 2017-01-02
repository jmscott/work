#
#  Synopsis:
#	Generate an html <table> that itemizes the contents of the web site.
#  Description:
#	Generate an html <table> element that is a manifest of the 
#	contents of the web site.  The top level *.cgi files are
#	read in $SERVER_ROOT/lib directory.
#  Note:
#	Makes more sense to parse a single xml file in
#	$SERVER_ROOT/manifest.cgi than scanning the lib directory.
#	Come the revolution.
#  Usage:
#	/cgi-bin/manifest?out=table
#  Depends
#	XML::Parser, &encode_html_entities, %QUERY_ARG
#
use XML::Parser;

our %QUERY_ARG;

my (
	@manifest,		# List of *.cgi files
	$parser,		# XML Parser
);

#
#  Find immediate child in XML tree.
#
sub find_kid
{
	my %arg = @_;
	my (
		$tag,
		$kids,
		$required,
		$what
	) = (
		$arg{tag},
		$arg{kids},
		$arg{required},
		$arg{what}
	);

	die 'find_kid: missing element tag' unless $tag;

	for (my $i = 1;  defined $kids->[$i];  $i += 2) {
		return $kids->[$i + 1] if $kids->[$i] eq $tag;
	}
	if ($required eq 'yes') {
		$what = 'child' unless $what;
		die "$what: can't find element $tag" if $required eq 'yes';
	}
	return undef;
}

#
#  Merge the child text nodes into single node,
#  since xml doesn't insure contiguous text always in a single node.
#
sub find_text
{
	my $node = $_[0];
	my $text;

	for (my $i = 1;  defined $node->[$i];  $i += 2) {
		#
		#  text node has name == 0
		#
		next unless $node->[$i] == 0;
		$text .= $node->[$i + 1];
	}
	#
	#  Strip leading and trailing text.
	#
	$text =~ s/^\s*|\s*$//g;

	return &encode_html_entities($text);
}

#
#  Go to $SERVER_ROOT/lib
#
my $lib = "$ENV{SERVER_ROOT}/lib";
die "chdir($lib) failed: $!" unless chdir($lib);

#
#  Open <table>.
#
#  Note: what about <caption>?
#
print <<END;
<table$QUERY_ARG{id_att}$QUERY_ARG{class_att}>
 <tr>
  <th>Name</th>
  <th>Title</th>
  <th>Files</th>
 </tr>
END

@manifest = sort <*.cgi>;

#
#  Create a single xml parser object for all *.cgi.
#
$parser = XML::Parser->new(
		Style => 'Tree'
	) or die "XML::Parser->new() failed: $!";
#
#  For each *.cgi, parse the xml file into a tree and
#  generate a <tr> in the <table> that links to the various
#  files called by the script.
#
for my $path (sort @manifest) {
	#
	#  Parse the XML/cgi description file.
	#  Need to surround parsefile() with an eval(),
	#  in case xml file is not well formed.
	#
	my $tree = $parser->parsefile($path) or
				die "XML::Parser->parsefile($path) failed: $!";
	#
	#  Kids match /cgi/*
	#
	my $kids = $tree->[1];

	#
	#  /cgi[@name] attribute
	#
	my $name = $kids->[0]->{name};

	#
	#  Find /cgi/title and link to help page.
	#  This implies the DTD will require a <title> element,
	#  a resonable assumption, come the revolution.
	#
	my $title_text = find_text(
				find_kid(
					tag	=>	'title',
					kids	=>	$kids,
					required=>	'yes',
				));
	my $cgi_bin_main_exists = -x "$ENV{SERVER_ROOT}/cgi-bin/$name";
	#
	#  Link to /cgi-bin/$name
	#
	my $title_twig;
	if ($cgi_bin_main_exists) {
		$title_twig =<<END;
<a
	title="$name"
	href="/cgi-bin/$name?help">$title_text</a>
END
	} else {
		$title_twig = $title_text;
	}
	#
	#  Link to source path.
	#
	my $path_a =<<END;
<a href="/file-view.shtml?path=lib/$name.cgi">lib/$name.cgi</a>
END
	#
	#  Link to automatically generated cgi-bin script.
	#
	my $main_twig;
	if ($cgi_bin_main_exists) {
		$main_twig =<<END;
, <a href="/file-view.shtml?path=cgi-bin/$name">cgi-bin/$name</a>
END
	}
		
	#
	#  Links to out/putters source programs.
	#
	my $putter_a;
	if (my $out = &find_kid(
				tag	=>	'out',
				kids	=>	find_kid(
					tag	=>	'GET',
					kids	=>	$tree->[1]
				))) {
		for (my $i = 1;  defined $out->[$i];  $i += 2) {
			next unless $out->[$i] eq 'putter';

			my $putter_name = $out->[$i + 1]->[0]->{name};
			$putter_a .=<<END;
, <a href="/file-view.shtml?path=lib/$name.d/$putter_name.pl">
	lib/$name.d/$putter_name.pl
</a>
END
		}
	}
	#
	#  Put <tr>
	#
	print <<END;
 <tr>
  <td>$name</td>
  <td>$title_twig</td>
  <td>$path_a$main_twig$putter_a</td>
 </tr>
END
}

#
#  Close <table>
#
print <<END;
</table>
END

1;
