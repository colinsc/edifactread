#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "edifact.h"


#define TAG_LEN   3

char s_terminator = '\'';
char separator = '+';
char component_separator = ':';
char release = '?';
char dpoint = '.';


char *segment_tag(char *segment, char *tag)
{
    // tag needs to point to a buffer at least 4 chars in capacity
    tag[TAG_LEN] = '\0';
    return strncpy(tag, segment, TAG_LEN);
}

struct transmission *initialize_transmission(void)
{
    struct transmission *t;

    if ((t = malloc(sizeof(struct transmission))) == NULL) {
        fprintf(stderr, "memory error initializing transmission\n");
        exit(EXIT_FAILURE);
    }

    t->message_count = 0;
    t->segment_count = 0;
    t->segments      = NULL;
    t->type[0]       = '\0';

    return t;
}

void append_segment(struct transmission *t, char *segbuf)
{
    //TBD check for malformed lengh segbuf must be at least 3 chars
    if (strncmp(segbuf,"UNH",TAG_LEN)==0) {
        t->message_count++;
        // msg type should be first component of second element
    }
    struct list_entry *ptr;

    if ((ptr = malloc(sizeof(struct list_entry))) == NULL) {
        fprintf(stderr, "memory error appending segment\n");
        exit(EXIT_FAILURE);
    }
    ptr->next = NULL;
    ptr->seg  = segbuf;

    struct list_entry *p;
    p = t->segments;
    if (p == NULL)
        t->segments = ptr;
    else {
        while (p->next != NULL)
            p = p->next;
        p->next = ptr;
    }
}

void free_transmission(struct transmission *t)
{
    struct list_entry *ptr;

    while (t->segments != NULL) {
        ptr = t->segments;
        t->segments = ptr->next;
        free(ptr->seg);
        free(ptr);
    }

    free(t);
}

/*
 * print_transmission : dump the raw segments from the transmission to stdout
 *
 */
void print_transmission(struct transmission *t)
{
    struct list_entry *s;

    s = t->segments;

    while (s != NULL) {
        printf("%s\n", s->seg);
        s = s->next;
    }
}

char *data(char *segbuf, int element, int component)
{
     /* for non composite elements the data is returned as component 0
      *  return ponter to requested element or component or
      *  NULL if not found
      */
     int offset = TAG_LEN + 1;
     int e =0; // element we are reading
     int c =0; // component we are reading
     char *ptr;
     ptr = segbuf + offset;    // set ptr after first +
     char *rptr = NULL;
     if (element == 0 && component == 0)
         rptr = ptr;
     while (*ptr) {
         if (*ptr == separator && *(ptr - 1) != release) {
             if (rptr)
                 return strndup(rptr, ptr - rptr);

             ++e;
             c = 0;
             if (e == element && component == c)
                 rptr = ptr + 1;
         }
         if (*ptr == component_separator && *(ptr - 1) != release) {
             ++c;
             if (rptr)
                 return strndup(rptr, ptr - rptr);
             if (e == element && component == c)
                 rptr = ptr + 1;
         }
         ++ptr;
     }
     if (rptr) {
         --ptr;
         return strndup(rptr, ptr - rptr);
     }

     return NULL; // No match;
}
