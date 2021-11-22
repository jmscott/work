#
#  Synopsis:
#	common routines for tag-url
#
use strict;
use utf8;
use IPC::Open2;

#  Code lifted from JSON-Tiny
my %ESCAPE = (
	'"'     => '"',
	'\\'    => '\\',
	'b'     => "\x08",
	'f'     => "\x0c",
	'n'     => "\x0a",
	'r'     => "\x0d",
	't'     => "\x09",
	'u2028' => "\x{2028}",		#  unicode line-separator
	'u2029' => "\x{2029}"		#  unicode paragraph-separator
);

my %REVERSE = map { $ESCAPE{$_} => "\\$_" } keys %ESCAPE;

for(0x00 .. 0x1f) {
	my $packed = pack 'C', $_;
	$REVERSE{$packed} = sprintf '\u%.4X', $_
			unless defined $REVERSE{$packed};
}

sub utf82json_string
{
	my $str = shift;
	$str =~ s!([\x00-\x1f\x{2028}\x{2029}\\"])!$REVERSE{$1}!gs;
	return "\"$str\"";
}

sub env2json()
{
	my $tab_indent = $_[0];

	my $tab1 = "\t" x $_[0];
	my $tab2 = $tab1 . "\t";

	my $json = "{\n";

	for (sort keys %ENV) {
		my $e = utf82json_string($_);
		my $v = utf82json_string($ENV{$_});
		$json .= "$tab2$e: $v,\n";
	}
	$json =~ s/,$//;
	return $json . "$tab1}";
}

1;
