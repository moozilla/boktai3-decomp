#include "global.h"

struct N { u8 f[0x20]; struct N *next; };
struct G { u8 f[0x18]; struct N *head; };
extern struct G *gUnk_02000028;
u32 sub_08005F40(struct N *);

s32 sub_08005F78(void)
{
    struct N *n;
    if (gUnk_02000028 == 0)
        return -1;
    n = gUnk_02000028->head;
    while (n != 0) {
        struct N *next = n->next;
        sub_08005F40(n);
        n = next;
    }
    return 0;
}
