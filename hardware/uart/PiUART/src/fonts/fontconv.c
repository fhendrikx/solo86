// taken from https://github.com/rsta2/circle/discussions/581#discussioncomment-13272765
// modified to support wider fonts

// gcc fontconv.c -o fontconv

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define COPY(n)         memcpy (Clipboard[0], Buffer[n], Header.bytesperglyph)
#define PASTE(n)        memcpy (Buffer[n], Clipboard[0], Header.bytesperglyph)

#define SWAP(n, m)      memcpy (Clipboard[1], Buffer[n], Header.bytesperglyph), \
                        memcpy (Buffer[n], Buffer[m], Header.bytesperglyph)     \
                        memcpy (Buffer[m], Clipboard[1], Header.bytesperglyph)

// https://wiki.osdev.org/PC_Screen_Font

#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF_font;

int main (int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: fontconv fontfile\n");
        return 1;
    }

    FILE *fin = fopen(argv[1], "r");
    if (fin == NULL) {
        fprintf(stderr, "Cannot open fontfile\n");
        return 1;
    }

    // read the header
    PSF_font Header;
    if (fread(&Header, sizeof Header, 1, fin) != 1) {
        fprintf(stderr, "Cannot read header\n");
        fclose(fin);
        return 1;
    }

    // check the font file looks legit
    if (Header.magic != PSF_FONT_MAGIC) {
        fprintf(stderr, "Unrecognised file format (incorrect magic number)\n");
        fclose(fin);
        return 1;
    }

    if (Header.numglyph != 256) {
        fprintf(stderr, "Expecting 256 glyphs, got %u\n", Header.numglyph);
        fclose(fin);
        return 1;
    }

    if (Header.width > 16) {
        fprintf(stderr, "This tool only handles fonts upto 16 pixels wide, this font is %u\n", Header.width);
        fclose(fin);
        return 1;
    }

    // read the font data
    uint8_t Buffer[256][Header.bytesperglyph];
    if (fread(Buffer, sizeof Buffer,  1, fin) != 1) {
        fprintf(stderr, "Cannot read font data\n");
        fclose(fin);
        return 1;
    }

    // To conform with Latin1 the characters 0x80 .. 0x9F of the original font have been
    // moved to 0xC0 .. 0xDF and have been replaced with character 0x00 (question mark).

    uint8_t Clipboard[2][Header.bytesperglyph];
    for (unsigned i = 0x00; i <= 0x1F; i++) { COPY (0x80 + i); PASTE (0xC0 + i); }

    COPY (0);
    for (unsigned i = 0x7F; i <= 0x9F; i++) PASTE (i);


    printf("//\n// %s\n//\n\n", argv[1]);
    printf("#include \"fonts.h\"\n\n");
    printf("// To conform with Latin1 the characters 0x80 .. 0x9F of the original font have been\n");
    printf("// moved to 0xC0 .. 0xDF and have been replaced with character 0x00 (question mark).\n\n");
    printf("static const unsigned char font_data[] = {\n");

    unsigned bytes_per_row = Header.bytesperglyph / Header.height;

    for (unsigned i = 33; i <= 255; i++) {
        printf("    ");

        for (unsigned j = 0; j < Header.height; j++) {

            switch (bytes_per_row) {
            case 1: {
                    uint8_t row = Buffer[i][j];
                    row = row >> (8 - Header.width);
                    printf ("0x%02X, ", Buffer[i][j]);
                }
                break;

            case 2: {
                    uint8_t row_high = Buffer[i][j * 2];
                    uint8_t row_low = Buffer[i][(j * 2) + 1];
                    uint16_t row = row_low | (row_high << 8);

                    row = row >> (16 - Header.width);

                    row_low = row & 0xFF;
                    row_high = row >> 8;

                    printf ("0x%02X, ", row_low);
                    printf ("0x%02X, ", row_high);
                }
                break;

            default:
                fprintf(stderr, "Bad bytes_per_row\n");
                fclose(fin);
                return 1;
            }

        }

        printf ("// 0x%02X\n", i);

    }

    printf("};\n\n");

    printf("const TFont FONTNAME = {\n");
    printf("    %u,    // width\n", Header.width);
    printf("    %u,    // height\n", Header.height);
    printf("    0,    // extraheight\n");
    printf("    0x21,    // first_char\n");
    printf("    0xFF,    // last_char\n");
    printf("    font_data\n");
    printf("};\n");

    fclose (fin);

    return 0;
}
