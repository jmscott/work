#
#  Synopsis:
#	Trivial Form Post.
#
our (
	%POST_VAR
);

print <<END;
Content-Type: text/html

The value of <code>form.$POST_VAR{in}</code> is '$POST_VAR{religion}'.
END
