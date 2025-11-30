#!/usr/bin/perl

use strict;

printf "static const u16 pCMap[256] = {\n";
printf "\t//        R    G    B\n";

open (my $csv_file, '<', 'vga-palette.csv') or die "?!";

my $idx = 0;
while (my $line = <$csv_file>) {

    chomp $line;

    # RGB decimal, RGB hex, web version
    my ($rd, $gd, $bd, $rh, $gh, $bh, $web) = split /,/, $line;

    my $r5 = $rd >> 3;
    my $g6 = $gd >> 2;
    my $b5 = $bd >> 3;

    #my $msb = $r5 << 3 | $g6 >> 3;
    #my $lsb = ($g6 & 0b111) << 5 | $b5;

    my $c = $r5 << 11 | $g6 << 5 | $b5;
    
    printf "\t// %3d  %3d  %3d  %3d\n", $idx, $r5, $g6, $b5;
    #printf "\t%d, %d,\n", $msb, $lsb;

    printf "\t%d,\n", $c;
    
    $idx++;
}

close ($csv_file);

printf "};\n";

