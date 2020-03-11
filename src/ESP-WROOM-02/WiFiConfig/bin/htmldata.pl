#!/usr/bin/env perl
#
# HTMLファイルからhtmldata.hを生成します。
# handle_send_XXXX という関数を呼び出すとレスポンスを返します。
#  
use feature qw( say );
use utf8;
use strict;
use warnings;

binmode(STDIN, ':utf8');
binmode(STDOUT,':utf8');
binmode(STDERR,':utf8');
 
use File::Basename;
use Cwd;

our $BASEDIR=Cwd::abs_path(dirname($0)."/../");

my $data=<<'__EOS__';
#ifndef _HTMLDATA_H_
#define _HTMLDATA_H_

__EOS__

my $prefix="HTMLDATA_";

sub html2header {
	my ($filename)=@_;
	my $buf="";
	open(my $fh,'<:utf8',$filename) || die "$filename - $!";
	{ local $/; $buf=<$fh> }

	$buf=~s/\\/\\\\/g;
	$buf=~s/\r?\n/\\n/g;
	$buf=~s/"/\\"/g;

	my $name=basename($filename);
	$name=~s/\./_/g;
	$name=~s/-/_/g;
	$name=uc($name);

	my $code=qq{#define $prefix$name "$buf"\n};
	$data.=$code;

	say qq{server.send(200, "text/html", $prefix$name);};
}

html2header("$BASEDIR/assets/wifisetup_index.html");
html2header("$BASEDIR/assets/wifisetup_index_post.html");
{
	open(my $fh,'>:utf8',"$BASEDIR/htmldata.h") || die "htmldata.h - $!";
	print $fh $data."\n#endif\n";
}

