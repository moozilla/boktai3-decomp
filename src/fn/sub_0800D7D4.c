#include "global.h"

struct N { u8 f[0xf0]; struct N *next; };
struct S { u8 f[0x1c]; struct N *head; };
extern u32 gUnk_0200046C;
void sub_0800CC50(struct N *);

u32 sub_0800D7D4(struct S *s)
{
    struct N *n = s->head;
    while (n != 0) {
        struct N *next = n->next;
        sub_0800CC50(n);
        n = next;
    }
    return gUnk_0200046C = 0;
}
