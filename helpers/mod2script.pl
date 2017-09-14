#!/usr/bin/perl
use warnings;
use strict;

my $filename = $ARGV[0];
open(INFILE, "<", $filename)
  or die "Could not open file '$filename' $!";

my $lineCount=0;
my $row;

my $pinCount=0;
my $powerPin=0;
my $GNDPin=0;

my @dip14 = (4,5,6,7,8,9,10,17,18,19,20,21,22,31);
my @dip16 = (4,5,6,7,8,9,10,11,16,17,18,19,20,21,22,31);
my @dip18 = (4,5,6,7,8,9,10,11,12,15,16,17,18,19,20,21,22,31);
my @dip20 = (4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,31);


while ($lineCount<5) {
	$row = <INFILE>;
#	chomp $row;
	$lineCount++;
	if ($lineCount==3)
	{
		$pinCount=$row;
		$pinCount =~ s/^\s+|\s+$//g;
	}
	if ($lineCount==4)
	{
		$powerPin=$row;
		$powerPin =~ s/^\s+|\s+$//g;
	}
	if ($lineCount==5)
	{
		$GNDPin=$row;
		$GNDPin =~ s/^\s+|\s+$//g;
	}
}
print "pins ".$pinCount."  Vcc ".$powerPin."  GND ".$GNDPin."\n";

if ($pinCount>20)
{
	die("ERROR! PINS >20!");
}

my @commands;

#switches up/down
if ($pinCount==16)
{
	push @commands, "11000000000000000000000000000000"."\n"; #sw4
}
if ($pinCount==18)
{
	push @commands, "10100000000000000000000000000000"."\n"; #sw5
}

my $command="00000000000000000000000000000000";

while ($lineCount<5+$pinCount) {
	$lineCount++;
	$row = <INFILE>;
	#chomp $row;
	my @q=split(',',$row);
	my $a="";
	if ($pinCount==14)
	{
		$a=$dip14[$lineCount-6];
	}
	if ($pinCount==16)
	{
		$a=$dip16[$lineCount-6];
	}
	if ($pinCount==18)
	{
		$a=$dip18[$lineCount-6];
	}
	if ($pinCount==20)
	{
		$a=$dip20[$lineCount-6];
	}
	
	my $b=$q[2];
	$b =~ s/^\s+|\s+$//g;
	if (($b==3)||($b==4)||($b==5))
	{
		$b="0";
	}
	#print $a." ".$b." ".$command."\n";
	substr($command,$a,1,$b);
	#print $command."\n";
}
push @commands, $command."\n";

while ($row = <INFILE>) {
	chomp $row;
	my @q=split(',',$row);
	$command="00000000000000000000000000000000";
	
	if (substr($q[1],0,1)==3)	#3 - send
	{
		substr($command,0,4,"0001");	#command
		my $lc=-1;
		my $h=$q[1];
		$h =~ s/^\s+|\s+$//g;
		$h =~ s/^.//; 
		$h=substr($h, 0, $pinCount);
		#print $h." - ";
		foreach my $chr (split //, $h) 
		{
			$lc++;
			my $a="";
			if ($pinCount==14)
			{
				$a=$dip14[$lc];
			}
			if ($pinCount==16)
			{
				$a=$dip16[$lc];
			}
			if ($pinCount==18)
			{
				$a=$dip18[$lc];
			}
			if ($pinCount==20)
			{
				$a=$dip20[$lc];
			}

			my $c=$chr;
			if (($c eq 'x')||($c eq 'X'))
			{
				$c='0';
			}
			#print $command." ".$a." ".$c." ".$lc."\n";
			substr($command,$a,1,$c);				
		}
		#print "\n".$command."\n\n";
		push @commands, $command."\n";
	}


	if (substr($q[1],0,1)==4)	#4 - read
	{
		substr($command,0,4,"0011");	#command
		my $lc=-1;
		my $h=$q[1];
		$h =~ s/^\s+|\s+$//g;
		$h =~ s/^.//; 
		$h=substr($h, 0, $pinCount);
		foreach my $chr (split //, $h) 
		{
			$lc++;
			my $a="";
			if ($pinCount==14)
			{
				$a=$dip14[$lc];
			}
			if ($pinCount==16)
			{
				$a=$dip16[$lc];
			}
			if ($pinCount==18)
			{
				$a=$dip18[$lc];
			}
			if ($pinCount==20)
			{
				$a=$dip20[$lc];
			}

			my $c=$chr;
			if (($c eq 'x')||($c eq 'X'))
			{
				$c='0';
			}
			substr($command,$a,1,$c);				
		}
		push @commands, $command."\n";
	}
	
	
	if (substr($q[1],0,1)==2)	#2 - io
	{
		substr($command,0,4,"0000");	#command
		my $lc=-1;
		my $h=$q[1];
		$h =~ s/^\s+|\s+$//g;
		$h =~ s/^.//; 
		$h=substr($h, 0, $pinCount);
		foreach my $chr (split //, $h) 
		{
			$lc++;
			my $a="";
			if ($pinCount==14)
			{
				$a=$dip14[$lc];
			}
			if ($pinCount==16)
			{
				$a=$dip16[$lc];
			}
			if ($pinCount==18)
			{
				$a=$dip18[$lc];
			}
			if ($pinCount==20)
			{
				$a=$dip20[$lc];
			}

			my $c=$chr;
			if (($c eq 'x')||($c eq 'X'))
			{
				$c='0';
			}
			substr($command,$a,1,$c);				
		}
		push @commands, $command."\n";
	}

}

foreach my $aaa (@commands)
{
	print $aaa;
}
print "\n";
my $sizeall=$#commands;
$sizeall+=2;
print $sizeall.", ";
foreach my $aaa (@commands)
{
	chomp $aaa;
	my $kk=0;
	my $val=0;
	foreach my $chr (split //, $aaa) 
	{
		if ($chr eq "1")
		{
			$val=$val + 2**$kk;
		}
		$kk++;
	}
	#print oct("0b" . $aaa).", ";
	print $val.", ";
}
print "\n";
