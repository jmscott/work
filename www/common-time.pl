#
#  Symopsis:
#	Convert elapsed seconds to terse english description
#  Usage:
#	$ago = elapsed_seconds2terse_english(time() - $mtime);
#

sub elapsed_seconds2terse_english
{
	my ($elapsed, $units)  = ($_[0]);

	#  > 1 year
	if ($elapsed > 31536000) {
		$elapsed /= 31536000;
		$units = 'yr';
	}

	#  > 3 months

	elsif ($elapsed > 7776000) {
		$elapsed /= 2592000;
		$units = 'mon';
	}

	#  > 3 weeks

	elsif ($elapsed > 1814400) {
		$elapsed /= 604800;
		$units = 'week';
	}

	#  > 3 days
	elsif ($elapsed > 259200) {
		$elapsed /= 86400;
		$units = 'day';

	#  > 3 hours

	} elsif ($elapsed > 10800) {
		$elapsed /= 3600;
		$units = 'hr';

	#  > 3 minutes

	} elsif ($elapsed > 180) {
		$elapsed /= 60;
		$units = 'min';
	} else {
		$units = 'sec';
	}
	return sprintf('%.f %s', $elapsed, $units);
}

1;
