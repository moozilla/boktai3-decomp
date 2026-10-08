#include "global.h"
struct E { u16 id; u8 pad; u8 pad1; u8 a; u8 b; u16 c; u16 d; u8 pad2[2]; u8 body[0x14]; struct E *next; };
struct G { u8 pad[0x18]; struct E *list; };
extern struct G *gUnk_02000028;
void sub_0821D698(u8 *, u32, u32, u32, u32, u32);
s32 sub_0800602C(u32 id)
{
    struct E *e;
    struct E *n;
    u16 k = id;
    if (gUnk_02000028 == 0) return -1;
    e = gUnk_02000028->list;
    while (e != 0) {
        n = e->next;
        if (e->id == k && e->pad == 0) {
            e->pad = 1;
            sub_0821D698(e->body, e->d, e->a, e->b, 0xff, e->c);
        }
        e = n;
    }
    return 0;
}
