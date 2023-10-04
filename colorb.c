#include "bmp.h"

#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_help_section(void) {
    fprintf(stdout, "Usage: colorb -i infile -o outfile\n       colorb -h\n");
    exit(0);
}

int main(int argc, char **argv) {
    Buffer *r, *w;
    int ch;

    bool _i = false;
    bool _o = false;
    bool _h = false;

    opterr = 0;

    while ((ch = getopt(argc, argv, "i:o:h")) != -1) {
        switch (ch) {
        case 'i':
            _i = true;
            r = read_open(optarg);
            if (!r) {
                fprintf(stderr, "colorb:  error reading input file %s\n", optarg);
                print_help_section();
            }
            break;
        case 'o':
            _o = true;
            w = write_open(optarg);
            break;
        case 'h': _h = true; break;
        default:
            fprintf(stderr, "colorb:  unknown or poorly formatted option %s\n", argv[1]);
            exit(0);
            break;
        }
    }

    if (!_i) {
        fprintf(stderr, "colorb:  -i option is required\n");
        print_help_section();
    }

    if (!_o) {
        fprintf(stderr, "colorb:  -o option is required\n");
        print_help_section();
    }

    if (_h) {
        print_help_section();
    } else {
        BMP *bmp = bmp_create(r);
        bmp_reduce_palette(bmp);
        bmp_write(bmp, w);
        bmp_free(&bmp);
        read_close(&r);
        write_close(&w);
        return 0;
    }
}
