#ifndef EDIFACT_H_
#define EDIFACT_H_

struct list_entry {
    char *seg;
    struct list_entry *next;
};

struct transmission {
    char type[7];
    int  message_count;
    int segment_count;
    struct list_entry *segments;
};

char *segment_tag(char *segment, char *tag);
struct transmission *initialize_transmission(void);
void append_segment(struct transmission *t, char *segbuf);
void free_transmission(struct transmission *t);
void print_transmission(struct transmission *t);
void debug_print_transmission(struct transmission *t);
char *data(char *segbuf, int element, int component);

extern char s_terminator;
extern char separator;
extern char component_separator;
extern char release;
extern char dpoint;

#endif
