#include "global.h"
struct E { u8 f[4]; u8 a; u8 g[0x5c - 5]; };
extern u32 gUnk_02000240;
void sub_08214514(void *);
void sub_081A46EC(u8 *p)
{
    struct E *e = (struct E *)(p + 0x5c);
    s32 i = 0x1f;
    do {
        if (e->a != 0) sub_08214514(e);
        e++;
        i--;
    } while (i >= 0);
    gUnk_02000240 = 0;
}
