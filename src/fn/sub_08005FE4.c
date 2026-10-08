#include "global.h"

struct N { u16 id; u8 f2; u8 p; u32 a[2]; u8 sub[1]; u8 f[0x13]; struct N *next; };
struct G { u8 f[0x18]; struct N *head; };
extern struct G *gUnk_02000028;
void sub_0821D6D0(void *);

s32 sub_08005FE4(u16 id)
{
    struct N *n;
    if (gUnk_02000028 == 0)
        return -1;
    n = gUnk_02000028->head;
    while (n != 0) {
        struct N *next = n->next;
        if (n->id == id && n->f2 != 0) {
            n->f2 = 0;
            sub_0821D6D0(n->sub);
        }
        n = next;
    }
    return 0;
}
