#include "global.h"

struct N { u8 f[5]; u8 id; u8 g[0xb2]; struct N *next; };
struct S { u8 f[0x28]; struct N *head; };
struct S *sub_08008270(void);
void sub_0800700C(struct N *);

s32 sub_08008950(u32 id)
{
    struct S *s = sub_08008270();
    struct N *n;
    if (s == 0)
        return -1;
    n = s->head;
    while (n != 0) {
        struct N *next = n->next;
        if (n->id == id)
            sub_0800700C(n);
        n = next;
    }
}
