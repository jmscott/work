#
#  Synopsis:
#	Make a working directory for a perl5 script.
#  See Also:
#	cgi2perl5-main
#  Depends:
#	Cwd.pm
#  Notes:
#	Depends upon the perl5 Cwd.pm modules.
#	Does the Cwd module need to be global?
#
#	What uses this file?
#
use Cwd;

our (
	%CGI
);

$CGI{boot_dir} = getcwd;

sub make_work_dir()
{
	return if $CGI{work_dir};	# work dir already made

	my $tmp = $ENV{TMPDIR} ? $ENV{TMPDIR} : '/tmp';
	#
	#  Build a directory using the cgi name, process id
	#  and parent process id.
	#
	#  Note: need to change parent process id to some form
	#  of date stamp.
	#
	$tmp = "$tmp/$CGI{name}-$$-" . getppid() . '.d';
	use File::Path;
	mkpath($tmp) or die "mkpath($tmp) failed: $!";
	$CGI{work_dir} = $tmp;
	chdir $CGI{work_dir} or die "chdir $CGI{work_dir} failed: $!";
}

sub remove_work_dir()
{
	$CGI{boot_dir} && $CGI{make_work_dir} && chdir($CGI{boot_dir}) &&
			rmtree($CGI{work_dir});
}

1;
