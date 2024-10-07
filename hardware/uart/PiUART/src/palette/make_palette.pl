#!/usr/bin/perl

use strict;

printf "GIMP Palette\n";
printf "Name: Dylan LCD 6 bit\n";
printf "Columns: 8\n";

my $i = 0;

for (my $r = 0; $r <= 3; $r++) {
    for (my $g = 0; $g <= 3; $g++) {
	for (my $b = 0; $b <= 3; $b++) {

	    my $r8 = $r << 6 | $r << 4 | $r << 2 | $r;
	    my $r5 = $r << 3 | $r << 1 | $r >> 1;

	    my $g8 = $g << 6 | $g << 4 | $g << 2 | $g;
	    my $g6 = $g << 4 | $g << 2 | $g;

	    my $b8 = $b << 6 | $b << 4 | $b << 2 | $b;
	    my $b5 = $b << 3 | $b << 1 | $b >> 1;

	    #printf "%d %d %d %d %d %d %d %d %d %d\n", $i, $r, $g, $b, $r8, $g8, $b8, $r5, $g6, $b5;

	    printf "%3d  %3d  %3d\n", $r8, $g8, $b8;

	    $i++;

	}
    }
}


