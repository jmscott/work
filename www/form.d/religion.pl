#
#  Synopsis:
#	Trivial Form Post.
#
our (
	%POST_VAR
);

print <<END;
Content-Type: text/html

The value of <form> $ENV{SCRIPT_NAME}.religion is '$POST_VAR{religion}'.
END
