#include "global.h"
struct G {
    u8 f[0x20];
    u8 a[0x28];
};
struct E {
    u8 f[0x12];
    u8 b;
    s8 idx;
    u8 pad[4];
    u32 c;
};
void Effect_Remove(void *);
void Memory_Clear(void *, u32);
void sub_08114E04(u8 *g, struct E *e)
{
    Effect_Remove(g + 0x20 + e->idx * 40);
    e->b = 0;
    e->c = 0;
    Memory_Clear(e, 0x1c);
    e->idx = 0xff;
}
