#include "global.h"
struct N { u8 p[0x78]; struct N *prev; struct N *next; }; struct L { u8 p[0x1c]; struct N *head; };
u32 sub_082323C0(struct L *l, struct N *n) { struct N *a = n->prev; struct N *b = n->next; if (a) a->next = b; else l->head = b; if (b) b->prev = a; return 0; }
