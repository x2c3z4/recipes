#!/usr/bin/perl


my $content;# = "<a>abc</a>";
#my $result;
my $filename = "a.html";


open(FILE, $filename) || die "Can't open file $filename : $!\n"; 
while($_=<FILE>) { $content = $content."$_";}
close(FILE); 
$_ = $content;

#($result) = ($content =~ /<a>(*)<\/a>$/);
#@result=($content =~ m|<a>(.*)</a>$|g);
s/\r|\n//g;
@result = m|<a[^<>]*?>([^<>]+?)</a>|g;
#$url="http://my.machine.tw:8080/cgi-bin/test.pl"; 
#($host, $port, $file)=($url =~ m|http://([^/:]+):{0,1}(\d*)(\S*)$|);
#print $content."\n";
print join "\n",@result;

#for($i=0; $i<=$#result; $i++) { print "$result[$i]\n"; }

