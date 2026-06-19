#
#  Synopsis:
#	Parse contents of $POST_DATA as json into $POST_VAR{data}.
#
use JSON::PP qw(decode_json);

our(
	$POST_DATA,
	%POST_VAR,

	#  Note:  rename to query_arg_{left,right}_RE!

	$left_RE,
	$right_RE,
);

die 'appliction/json: empty json sent' unless length($POST_DATA) > 0;

#
#  POST of application/json comprise only single json "blob".
#  Consequently, we have no way to know where to "route" the request to
#  what is typically define in the "in" variable.  However, curl allows
#  the -d argument as part of the request.  unfor
#

$POST_VAR{json} = decode_json($POST_DATA);
die 'appliction/json: can not parser json: ' . substr($POST_DATA, 0, 32)
	unless $POST_VAR{json}
;

die 'query string does not define var "in"'
	unless $ENV{QUERY_STRING} =~ /${left_RE}in$right_RE/
;
$POST_VAR{in} = $1;

1;
