#include "global.h"

struct N { u16 id; u8 f[0x1e]; struct N *next; };
struct G { u8 f[0x18]; struct N *head; };
extern struct G *gUnk_02000028;
u32 sub_08005F40(struct N *);

s32 sub_08005FA8(u16 id)
{
    struct N *n;
    if (gUnk_02000028 == 0)
        return -1;
    n = gUnk_02000028->head;
    while (n != 0) {
        struct N *next = n->next;
        if (n->id == id)
            sub_08005F40(n);
        n = next;
    }
    return 0;
}
