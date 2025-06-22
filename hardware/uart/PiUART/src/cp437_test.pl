#!/usr/bin/perl

use strict;

for (my $i = 128; $i <= 255; $i++) {

    printf(chr($i));

    if ($i % 16 == 15) {
        printf("\n");
    }

}