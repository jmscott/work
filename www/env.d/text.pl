#
#  Synopsis:
#	Write the value of an environment variable or query argument as text.
#  Usage:
#	/cgi-bin/env?out=text&var=blob
#	/cgi-bin/env?out=text&var=PATH&src=QS
#  Depends:
#	&encode_html_entities(), %QUERY_ARG
#  Note:
#	How to be sure value of ENV is utf8?
#
use utf8;

require 'httpd2.d/common.pl';

our (
	%QUERY_ARG,
	$left_RE,		#  left query arg regex.  need to be renamed.
	$right_RE
);

my $name = $QUERY_ARG{name};
my $src = $QUERY_ARG{src};

#
#  Source is an process environment variable.
#
#  Note:
#	How do we know value is utf8?
#
if ($src eq 'env') {
	if (defined $ENV{$name}) {
		print &encode_html_entities($ENV{$name});
	}
} elsif ($ENV{QUERY_STRING} =~ /${left_RE}$name$right_RE/) {
	print &encode_html_entities(&decode_url_query_arg($1));
}

return 1;
