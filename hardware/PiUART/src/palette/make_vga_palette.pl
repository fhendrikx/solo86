#!/usr/bin/perl

use strict;

printf "GIMP Palette\n";
printf "Name: VGA 8 bit\n";
printf "Columns: 8\n";

open (my $csv_file, '<', 'vga-palette.csv') or die "?!";

while (my $line = <$csv_file>) {

    chomp $line;

    # RGB decimal, RGB hex, web version
    my ($rd, $gd, $bd, $rh, $gh, $bh, $web) = split /,/, $line;

    printf "%3d  %3d  %3d\n", $rd, $gd, $bd;

}

close ($csv_file);
