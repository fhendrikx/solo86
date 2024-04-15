#!/usr/bin/perl

use strict;
use warnings;

printf STDERR "Reading %s\n", $ARGV[0];

# open the file
open(my $fh, '<', $ARGV[0]) or die "$!";
binmode($fh);

# print the header
printf "u8 font [] = {\n";

# read and print the bytes
my $byte;
my $addr = 0;
while (read($fh, $byte, 1))
{

    my $ubyte = unpack("C", $byte);
    printf " 0x%0.2x,", $ubyte;
    printf "\n" if ($addr % 16 == 15);
    $addr++;
}

close($fh);

# print the footer
printf "};\n";

