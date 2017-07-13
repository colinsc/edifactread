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


void segment_tag(char *segment, char *tag)
{
    // tag needs to point to a buffer at least 4 chars in capacity
    strncpy(tag, segment, TAG_LEN);
    tag[TAG_LEN] = '\0';
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
