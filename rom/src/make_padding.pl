#!/usr/bin/perl

use strict;

for (my $i = 0; $i < 16; $i++) {

    for(my $j = 0; $j < 65536; $j++) {

        print chr($i);
    }

}


