#include "global.h"
struct E { u8 f[4]; u8 a; u8 g[0x90 - 5]; };
extern s32 gUnk_02000238;
void sub_08214514(void *);
void sub_0821FE6C(void *);
s32 sub_081A3468(struct E *e)
{
    s32 i;
    e = (struct E *)((u8 *)e + 0x48);
    i = 0x1f;
    do {
        if (e->a != 0) {
            sub_08214514(e);
            sub_0821FE6C((u8 *)e + 0x2c);
        }
        e++;
        i--;
    } while (i >= 0);
    return gUnk_02000238 = 0;
}
