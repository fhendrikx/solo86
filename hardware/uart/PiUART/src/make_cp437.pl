#!/usr/bin/perl

use Text::Iconv;
use strict;

my $converter = Text::Iconv->new("CP437", "UTF-8");

printf("#include \"cp437.h\"\n\n");
printf("const char *cp437_conv[] = {\n");

for (my $i = 128; $i <= 255; $i++) {

    my $converted = $converter->convert(chr($i));

    printf("    \"");

    for my $c (split('', $converted)) {
        printf("\\x%X", ord($c));
    }

    printf("\", ");

    if (length $converted == 2) {
        printf("    ");
    }

    printf("// %d: %s ", $i, $converted);

    printf("\n");

}

printf("};\n");

