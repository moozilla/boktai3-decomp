#include "global.h"

struct N { u8 f[0xb8]; struct N *next; };
struct S { u8 f[0x28]; struct N *head; };
extern u32 gUnk_02000030;
void sub_0800700C(struct N *);

u32 sub_08008148(struct S *s)
{
    struct N *n = s->head;
    while (n != 0) {
        struct N *next = n->next;
        sub_0800700C(n);
        n = next;
    }
    return gUnk_02000030 = 0;
}
