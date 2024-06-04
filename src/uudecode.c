#include <stdio.h>
#include <string.h>

/* Macro for decoding */
#define DEC(c) (((c) == '`' ? ' ' : (c)) - ' ' & 077)

void decode(in, out)
FILE *in;
FILE *out;
{
    char buf[80]; /* Input line buffer */
    unsigned char decoded[45]; /* Decoded data buffer */
    int i, n;

    /* Read each line of input file */
    while (fgets(buf, sizeof(buf), in)) {
        /* Skip lines starting with "begin" or "end" */
        if (strncmp(buf, "begin", 5) == 0 || strncmp(buf, "end", 3) == 0)
            continue;

        /* Determine number of bytes to decode from first character of line */
        n = DEC(buf[0]);
        if (n <= 0)
            continue;

        /* Decode each group of four characters into three bytes */
        for (i = 0; i < n; i += 3) {
            unsigned char c1 = DEC(buf[1 + i * 4 / 3]);
            unsigned char c2 = DEC(buf[2 + i * 4 / 3]);
            unsigned char c3 = DEC(buf[3 + i * 4 / 3]);
            unsigned char c4 = DEC(buf[4 + i * 4 / 3]);

            /* Write decoded bytes to buffer */
            if (i < n) decoded[i] = (c1 << 2) | (c2 >> 4);
            if (i + 1 < n) decoded[i + 1] = (c2 << 4) | (c3 >> 2);
            if (i + 2 < n) decoded[i + 2] = (c3 << 6) | c4;
        }

        /* Write decoded characters to output file */
        fwrite(decoded, 1, n, out);
    }
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

    /* Open input file in text mode and output file in binary mode */
    infile = fopen(argv[1], "r");
    outfile = fopen(argv[2], "wb");

    /* Check if files opened successfully */
    if (!infile || !outfile) {
        printf("Error opening files.\n");
        return 1;
    }

    /* Decode file */
    decode(infile, outfile);

    /* Close files */
    fclose(infile);
    fclose(outfile);

    return 0;
}