#
#  Synopsis:
#	Call perl's version of the c library system().
#
#  Usage:
#	do_system(
#		command		=>	'psql -f abc.sql',
#		out_file	=>	'psql.out',
#		die_empty_out	=>	'yes',
#		die_full_err	=>	'yes',
#		return_out	=>	'yes',
#		err_file	=>	'psql.err',
#		debug		=>	'yes'		
#		ignore_system_non_zero		=>	'yes'		
#	)
#
#  Note:
#	Need to think about call back functions for error and each row.
#
#	The arguments die_empty_out and die_full_err should be
#	replaced by err_callback and out_callback that reference the die
#	function.
#
sub do_system
{
	my %args = @_;

	my $debug = $args{debug} eq 'yes';
	my $command = $args{command};
	warn "command=$command" if $debug;
	die "argument command is empty" unless $command;

	#
	#  Build unix shell command, concatenating out and error files.
	#
	#  Note: if we are expected to return the output of the script as a
	#	 scalar, i.e., return_out is defined, then out_file must
	#	 also be defined.
	#
	my ($out_file, $return_out) = ($args{out_file}, $args{return_out});
	die "return_out required out_file to be defined"
						if $return_out && !$out_file;
	$command = sprintf('%s >%s', $command, $out_file) if $out_file;
	my $err_file = $args{err_file};
	$command = sprintf('%s 2>%s', $command, $err_file) if $err_file;
	warn "system($command)" if $debug;

	#
	#  Run the command, burping out the error file to STDERR.
	#  If the arg die_full_err equals 'yes' then die, as well.
	#
	my $status = system($command);
	warn "system returned $status" if $debug;
	if ($err_file && -s $err_file > 0) {
		warn "err_file $err_file is not empty" if $debug;
		#
		#  Shouldn't DO_SYSTEM_ERROR be my?
		#
		open(DO_SYSTEM_ERROR, "<$err_file") or
					die "open(<$err_file) failed: $!";
		my $buf;
		sysread(DO_SYSTEM_ERROR, $buf, 40) > -1 or
					die "sysread($err_file) failed: $!";
		close DO_SYSTEM_ERROR or die "close $err_file failed: $!";
		warn "system() returned status=$status" if $debug;
		warn "command: $command" if $debug;
		die $buf if $args{die_full_err} eq 'yes';
	}
	die "system($command) returned non-zero status: $status: $!"
		if $status != 0 && $args{ignore_system_non_zero} ne 'yes';
	#
	#  If the output file is empty and we expect output, then croak.
	#
	die "unexpected empty stdout: $out_file"
				if $args{die_empty_out} && -s $out_file == 0;
	#
	#  Just return 0 unless the caller wants the returned value.
	#
	return 0 unless $args{return_out} eq 'yes';

	#
	#  Caller wants the contents of stdout returned in scalar.
	#
	warn ("return contents of $out_file, size=" . -s $out_file) if $debug;

	open(DO_SYSTEM_OUT, "<$out_file") or die "open(<$out_file) failed: $!";
	my ($buf, $buf2);
	$buf .= $buf2 while ($status = sysread(DO_SYSTEM_OUT, $buf2, 8096)) > 0;
	warn "read: $buf" if $debug;
	die "sysread($out_file) failed: $!" if $status < 0;
	close DO_SYSTEM_OUT or die "close($out_file) failed: $!";
	warn "bye" if $debug;
	return $buf;
}

1;
