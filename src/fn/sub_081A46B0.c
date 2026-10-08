#include "global.h"
struct E { u16 a; u16 b; u8 f[5]; u8 c; u8 g[0x5c - 10]; };
extern const u32 gUnk_08611F40[];
void sub_0824923C(void *, u32);
void sub_081A46B0(u8 *p)
{
    void *q = p + 0x5c;
    const u32 *t = gUnk_08611F40;
    s32 i = 0x1f;
    struct E *e = (struct E *)(p + 0xac);
    do {
        if (e->a != 0) {
            sub_0824923C(q, t[e->c]);
            e->b++;
        }
        e++;
        q = (u8 *)q + 0x5c;
        i--;
    } while (i >= 0);
}
