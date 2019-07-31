#
#  Synopsis:
#	Trivial Form Post.
#
our (
	%POST_VAR
);

print <<END;
Content-Type: text/plain

The value of <code>religion</code> is '$POST_VAR{religion}'.
END
