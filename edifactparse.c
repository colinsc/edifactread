#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "edifact.h"

void usage(char *);
size_t read_file(char *filename, char **buf);
void parse_file(char *buf, size_t buflen);
void add_segment(struct transmission *trans, char *seg_start, int seglen);
void check_service_string_advice(char *buf, size_t buflen);

enum output_fmt {SEGMENT_PER_LINE, COMPONENT_PER_LINE, DOCUMENTED};
static enum output_fmt output_format = SEGMENT_PER_LINE;   // governs format of output

int main(int argc, char *argv[])
{
    if (argc > 1) {
	 for (int i = 1; i < argc; i++) {
             if (strcmp(argv[i],"-d") == 0)
                 output_format = COMPONENT_PER_LINE;
             else if (strcmp(argv[i],"-D") == 0)
                 output_format = DOCUMENTED;
             else {
                 char *buf = NULL;
                 size_t filelen;
                 filelen = read_file(argv[i], &buf);
                 parse_file(buf, filelen);
                 free(buf);
             }
	 }
    }
    else {
        usage(argv[0]);
        exit(2);
    }
    return EXIT_SUCCESS; 
}

void usage(char *program_name)
{
    printf("Usage: %s [-d] [filename ..]\n  -d print component per line (default is segment\n", program_name);
}

size_t read_file(char *filename, char **buf)
{
    FILE *fh;
    size_t file_size = 0;

    if ((fh=fopen(filename,"r"))!=NULL) {
	 if (fseek(fh, 0, SEEK_END) != 0) {
	      fprintf(stderr, "Seek error on %s\n", filename);
	      exit(EXIT_FAILURE);
	 }

	 file_size = (size_t) ftell(fh);
	 fseek(fh, 0, SEEK_SET);
	 char *b;
	 b = malloc(file_size);
	 fread(b, 1, file_size, fh);
	 fclose(fh);
	 *buf = b; // return read buffer to caller
    } else {
	 fprintf(stderr, "Error reading %s\n",filename);
	 exit(EXIT_FAILURE);
    }
    return file_size;
}
void parse_file(char *buf, size_t buflen)
{
     check_service_string_advice(buf, buflen);
     size_t i = 0;
     size_t last = 0;
     size_t segstart = 0;

     struct transmission *t;
     t = initialize_transmission();
     
     while ( i < buflen) {
	  if (buf[i] == s_terminator && i != 0 && buf[last] != release) {
	       // next seg starts at i+1 if not file end
	       add_segment(t, buf + segstart, i - segstart + 1);
	       segstart = i + 1;
	  }
          last = i;
	  ++i;
     }
     if (output_format == COMPONENT_PER_LINE)
         debug_print_transmission(t);
     else if (output_format == DOCUMENTED)
         debug_print_transmission(t);
     else
         print_transmission(t);
     free_transmission(t);
     
}

void add_segment(struct transmission *trans, char *seg_start, int seglen)
{
     char *seg;

     seg = strndup(seg_start, seglen);
     append_segment(trans, seg);
     
}

void check_service_string_advice(char *buf, size_t buflen)
{
#define UNA_LENGTH 9
    if (buflen < UNA_LENGTH) {
        fprintf(stderr, "Transmission too short to include service string advice\n");
        exit(EXIT_FAILURE);
    }
    // This is the default UNA segment "UNA:+.? '";
    // Set the various separators from the UNA segment
    component_separator = buf[3];
    separator           = buf[4];
    dpoint              = buf[5];
    release             = buf[6];
    s_terminator        = buf[8];
}
