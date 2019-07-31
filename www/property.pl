#
#  Synopsis:
#	Parse a property file into the hash array.
#
#  Returns:
#	Hash table with properties.
#
#  Usage:
#	%CONFIG = &load_property(
#		source_path	=>	'etc/enginfo2.conf'
#	);
#	print "Database is: $CONFIG{postgres.db.name}\n";
#
#  Description:
#	Parse a properties file into a % hash array.
#	The variables are typical name=value pairs, allowing the '.'
#	character to segregate name spaces.
#
#	The entries are parsed one line at a time and cannot span
#	a single line.  Typical entries look like:
#
#		name=abc
#		postgres.db.omegalx88=enabled
#		postgres.db.omegalx88.PGHOST = 10.214.34.88
#
#	Leading white space at the start of the line is ignored
#	and white space on either side of the first '=' character is
#	stripped.  Lines introduced with # are comments and ignored,
#	as well as lines with white space.  Any other line is a syntax
#	error that will cause a run time error.
#
#	Variables with 'ENV' as the first component are meant to be 
#	exported into the unix process environment:
#
#	       ENV.LD_LIBRARY_PATH=/usr/local/lib:/usr/lib:/lib
#	       ENV.PATH=/usr/local/bin:/usr/bin:/bin
#
#	exports the ubiquitous LD_LIBRARY_PATH and PATH variables
#	for use by child processes.
#
#	The pattern ${name} in a value is substitutes the value of a 
#	previously scanned variable.  Inherited environment variables may 
#	be referenced, as well, allowing constructs like:
#
#		ENV.PATH=${SERVER_ROOT}/bin:${ENV.PATH}
#
#	The ${name} pattern is expanded immediately and so is not recursive,
#	following the convention of the Bourne shell.
#  Depends:
#	nothing
#

#
#  Regular expression to match property name.
#
my $name_RE = '\w+(?:\.\w+){0,254}';

sub load_property
{
	my %arg = @_;

	my (
		$source_path,
		%property
	) =  (
		$arg{source_path}
	);

	die "load_property: missing source path" unless $source_path;

	open(SOURCE_PATH, "<$source_path") or
			die "load_property: open($source_path) failed: $!";
	my $line_count = 0;

	#
	#  Read each line from the config file
	#
	while (<SOURCE_PATH>) {
		chop;			# truncate trailing newline/cr cruft
		$line_count++;

		next if m/^\s*#/ || m/^\s*$/;	# comment or blank line

		die "load_property: $source_path: can't parse line $line_count"
					unless m/^\s*($name_RE)\s*=\s*(.*)$/;
		my (
			$name,
			$value,
		) = ($1, $2);
		#
		#  Insure the property is not defined twice.
		#
		die "load_property: $name: property assigned again " .
				"near line $line_count" if $property{$name};
		#
		#  Loop over all the matching ${name} patterns in the 
		#  regular expression and substitute ${name} with
		#  the property value.  Notice that the match always 
		#  begins on the next pattern, so an unresolved ${name} 
		#  doesn't cause an infinite loop.
		#
		while ($value =~ m/\${($name_RE)}/g) {
			my $n = $1;

			#
			#  If name begins with 'ENV.', then search %ENV,
			#  the process environment hash;  otherwise, search
			#  the current process table.
			#
			if (substr($n, 0, 4) eq 'ENV.') {
				$value =~ s/\${$n}/$ENV{substr($n, 4)}/;
			} elsif ($property{$n}) {
				$value =~ s/\${$n}/$property{$n}/;
			}
		}
		#
		#  If name begins with 'ENV.',
		#  then directly export to the process environment;
		#  otherwise, add to the property hash table.
		#
		if (substr($name, 0, 4) eq 'ENV.') {
			$ENV{substr($name, 4)} = $value;
		} else {
			$property{$name} = $value;
		}
	}
	return %property;
}

1;
