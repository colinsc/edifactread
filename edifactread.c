#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
 * Simple Edifact read and display
 * reads the files passed on the command line and
 * prints them out on std out with a CR
 * after each segment
 */

void usage(char *);
void process_file(char *);

int main(int argc, char *argv[])
{
    if (argc > 1) {
        for (int i = 1; i < argc; i++)
            process_file(argv[i]);
    }
    else {
        usage(argv[0]);
        exit(2);
    }
    return 0;
}

void usage(char *program_name)
{
    printf("Usage: %s [filename ..]\n", program_name);
}

void process_file(char *filename)
{
    //printf("processing %s\n", filename);
    FILE *fh;

    if ((fh=fopen(filename,"r"))) {
        int c;
        int prev = 0;
        while ((c = fgetc(fh)) != EOF) {
            if (c == '\'' && prev != '?') {
                putchar(c);
                putchar('\n');
            }
            else
                putchar(c);
            prev = c;
        }
        fclose(fh);
    }
}
