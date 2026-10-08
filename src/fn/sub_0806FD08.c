#include "global.h"
struct N { struct N *next; u8 *obj; };
extern struct N *gUnk_020004AC;
void sub_08249240(u8 *, u32, u32);
void sub_0806FD08(u8 a, u32 b, u16 c)
{
    struct N *n = gUnk_020004AC;
    if (n != 0) {
        while (n->next != 0) {
            u8 *o = n->obj;
            u32 t;
            if (*(u8 *)(o + 0x98) == a && *(u32 *)(o + 0x50) == c) {
                if ((t = *(u32 *)(o + 0x24c)) != 0) {
                    sub_08249240(o, b, t);
                    return;
                }
            }
            n = n->next;
        }
    }
}
