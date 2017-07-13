#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void usage(char *);
size_t read_file(char *filename, char **buf);
void parse_file(char *buf, size_t buflen);
void add_segment(char *seg_start, int seglen);
void check_service_string_advice(char *buf, size_t buflen);

char s_terminator = '\'';
char separator = '+';
char component_separator = ':';
char release = '?';
char dpoint = '.';

int main(int argc, char *argv[])
{
    if (argc > 1) {
	 for (int i = 1; i < argc; i++) {
	     char *buf = NULL;
	     size_t filelen;
	     filelen = read_file(argv[i], &buf);
	     parse_file(buf, filelen);
             free(buf);
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
    printf("Usage: %s [filename ..]\n", program_name);
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
     
     while ( i < buflen) {
	  if (buf[i] == s_terminator && i != 0 && buf[last] != release) {
	       // next seg starts at i+1 if not file end
	       add_segment(buf + segstart, i - segstart + 1);
	       segstart = i + 1;
	  }
          last = i;
	  ++i;
     }
     
}

void add_segment(char *seg_start, int seglen)
{
     char *seg;

     seg = strndup(seg_start, seglen);

     printf("%s\n", seg);
     free(seg);
     
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
