#
#  Synopsis:
#	Trivial Form Post.
#
our (
	%POST_VAR
);

print <<END;
Content-Type: text/html

The value of <form> $POST_VAR{in}.religion is '$POST_VAR{religion}'.
END
