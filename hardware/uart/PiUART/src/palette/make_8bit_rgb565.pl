#!/usr/bin/perl

use strict;

printf "static const uint8_t cmap[256][2] = {\n";
printf "\t//        R    G    B\n";

my $i = 0;

for (my $r = 0; $r <= 3; $r++) {
    for (my $g = 0; $g <= 3; $g++) {
	for (my $b = 0; $b <= 3; $b++) {

	    my $r5 = $r << 3 | $r << 1 | $r >> 1;
	    my $g6 = $g << 4 | $g << 2 | $g;
	    my $b5 = $b << 3 | $b << 1 | $b >> 1;

	    my $msb = $r5 << 3 | $g6 >> 3;
	    my $lsb = ($g6 & 0b111) << 5 | $b5;

	    printf "\t// %3d  %3d  %3d  %3d\n", $i, $r5, $g6, $b5;
	    printf "\t%d, %d,\n", $msb, $lsb;

	    $i++;

	}
    }
}

for (; $i <= 255; $i++) {
    printf "\t// %3d  %3d  %3d  %3d\n", $i, 0, 0, 0;
    printf "\t%d, %d,\n", 0, 0;
}

printf "};\n";
