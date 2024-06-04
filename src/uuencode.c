#include <stdio.h>
#include <string.h>

/* Macros for encoding and replacing SPACE with the GRAVE character */
#define ENC(c) (((c) & 077) + ' ')
#define ENC_SPACE(c) ((c) == ' ' ? '`' : (c))

void encode(in, out, filename)
FILE *in;
FILE *out;
char *filename;
{
    int i, n;
    unsigned char buf[45];

    /* Write header line */
    fprintf(out, "begin 644 %s\n", filename);

    /* Read input file in chunks of 45 bytes and encode each chunk  */
    while ((n = fread(buf, 1, 45, in)) > 0) {
        putc(ENC_SPACE(ENC(n)), out);

        /* Encode each three byte chunk */
        for (i = 0; i < n; i += 3) {
            unsigned char c1 = buf[i];
            unsigned char c2 = (i + 1 < n) ? buf[i + 1] : 0;
            unsigned char c3 = (i + 2 < n) ? buf[i + 2] : 0;

            /* Write encoded characters to output file */
            putc(ENC_SPACE(ENC(c1 >> 2)), out);
            putc(ENC_SPACE(ENC(((c1 << 4) & 060) | ((c2 >> 4) & 017))), out);
            putc(ENC_SPACE(ENC(((c2 << 2) & 074) | ((c3 >> 6) & 03))), out);
            putc(ENC_SPACE(ENC(c3 & 077)), out);
        }
        putc('\n', out);
    }

    /* Write footer line */
    fprintf(out, "`\nend\n");
}

int main(argc, argv)
int argc;
char *argv[];
{
    FILE *infile, *outfile;

    /* Check for correct number of arguments */
    if (argc != 3) {
        printf("Usage: %s inputfile outputfile\n", argv[0]);
        return 1;
    }

    infile = fopen(argv[1], "rb");
    outfile = fopen(argv[2], "w");

    /* Check if files opened successfully */
    if (!infile || !outfile) {
        printf("Error opening files.\n");
        return 1;
    }

    /* Encode file */
    encode(infile, outfile, argv[1]);

    /* Close files */
    fclose(infile);
    fclose(outfile);

    return 0;
}