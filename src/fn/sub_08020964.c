#include "global.h"

struct N { u16 id; u8 a; u8 f[0x29]; struct N *prev; struct N *next; };
struct S { u8 f[0x74]; struct N *head; };
struct N *sub_08020964(struct S *p, u32 id, u32 a)
{
    struct N *n = p->head;
    while (n) {
        struct N *next = n->next;
        if (n->id == id && n->a == a)
            return n;
        n = next;
    }
    return 0;
}
