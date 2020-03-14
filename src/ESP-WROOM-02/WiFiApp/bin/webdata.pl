#!/usr/bin/env perl
# Webデータ生成
use feature qw( say );
use utf8;
use strict;
use warnings;

use File::Basename;
use Cwd;

binmode(STDIN, ':utf8');
binmode(STDOUT,':utf8');
binmode(STDERR,':utf8');
 
our $BASEDIR=Cwd::abs_path(dirname($0)."/../");

my $assets="$BASEDIR/assets";

my @targets=(
	[ "WiFiSetupIndex", "webui/var/build/public/index.html" ],
);

sub file_write {
	my ($fn,$data)=@_;
	open(my $fh,">:utf8",$fn) || die "$fn - $!";
	print $fh $data;
	say "Write: ".basename($fn);
}

sub file_read {
	my ($fn)=@_;
	open(my $fh,"<:utf8",$fn) || die "$fn - $!";
	local $/;
	say "Read: ".basename($fn);
	return <$fh>;
}

{
	my $buf=<<'__EOS__';
#ifndef _WEBDATA_H_
#define _WEBDATA_H_
#include "Arduino.h"
__EOS__
	foreach(@targets) {
		$buf.="String WebData$_->[0]();\n";
	}
	$buf.=<<'__EOS__';
#endif
__EOS__
	file_write("$BASEDIR/WebData.h",$buf);
}

{
	my $buf=qq{#include "WebData.h"\n\n};
	foreach(@targets) {
		my $data=file_read($_->[1]);
		$data=~s/\\/\\\\/g;
		$data=~s/\r?\n/\\n/g;
		$data=~s/"/\\"/g;
		$buf.="String WebData$_->[0]() {\n";
		$buf.=qq{\treturn F("$data");\n\}\n};
	}
	file_write("$BASEDIR/WebData.cpp",$buf);
}

