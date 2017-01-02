#
#  Synopsis:
#	Write html text of full file system path of the path query argument.
#  Usage:
#	/cgi-bin/file?out=text.full-path
#  Depends:
#	&encode_html_entities(), %QUERY_ARG
#
our %QUERY_ARG;

print &encode_html_entities("$ENV{SERVER_ROOT}/$QUERY_ARG{path}");
