#include "global.h"
struct E { u8 f[4]; u8 a; u8 g[0x178 - 5]; };
extern s32 gUnk_0200023C;
void sub_08214514(void *);
void sub_08013B74(void *);
void sub_0821FE6C(void *);
void sub_081A3D50(struct E *e)
{
    s32 i;
    e = (struct E *)((u8 *)e + 0x48);
    i = 2;
    do {
        if (e->a != 0) {
            sub_08214514(e);
            sub_08013B74((u8 *)e + 0xec);
            sub_0821FE6C((u8 *)e + 0x3c);
        }
        e++;
        i--;
    } while (i >= 0);
    gUnk_0200023C = 0;
}
