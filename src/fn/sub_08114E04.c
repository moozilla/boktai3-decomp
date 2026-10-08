#include "global.h"
struct G { u8 f[0x20]; u8 a[0x28]; };
struct E { u8 f[0x12]; u8 b; s8 idx; u8 pad[4]; u32 c; };
void sub_08217EAC(void *);
void sub_08219DD8(void *, u32);
void sub_08114E04(u8 *g, struct E *e)
{
    sub_08217EAC(g + 0x20 + e->idx * 40);
    e->b = 0;
    e->c = 0;
    sub_08219DD8(e, 0x1c);
    e->idx = 0xff;
}
