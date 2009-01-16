#!/usr/bin/perl

use English;
use strict;
use warnings;

sub getClip() {
	open(FH, $ENV{'HOME'} . "/.blobfs/rootclip")
		|| die "Can't open rootclip";

	while (<FH>) {
		my $clip = $_;

		$clip =~ s/rootdir.clip = // || next;

		chomp($clip);
		close(FH);

		return $clip;
	}

	die "Can't find rootclip";
}

sub invert($) {
	my $clip = shift;
	my $md5 = substr($clip, 0, 27);
	my $guid = substr($clip, 27);

	return $guid . $md5;
}

sub getClipFile($) {
	my $clip = shift;
	my @clips = glob("/tmp/diskconnector*/real/$clip");

	return $clips[0];
}

sub main($) {
	my $clip = shift;

	if (! defined($clip)) {
		$clip = getClip();
	}

#	my $clip = getClip();
#	my $clip = shift;
	my $normClip = invert($clip);
	my $file = getClipFile($normClip);

	open(FH, $file)
		|| die "Can't open file '$file'";
	while (<FH>) {
		print $_;
#		/eclipblob/ && print $_;
	}
	close(FH);
}
main($ARGV[0]);
