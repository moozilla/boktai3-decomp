#include "global.h"

extern u32 gUnk_03005308;
extern u8 gUnk_0203B400[];
struct P { u8 f[0x170]; void (*cb)(void); u8 g[0x184 - 0x174]; u32 a; };
void sub_080F4B94(void);

void sub_080F4B54(struct P *p)
{
    u32 v;
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
    v = *(u16 *)(gUnk_0203B400 + gUnk_03005308 * 2);
    p->a = (v & 0x40) + 0x1e;
    p->cb = sub_080F4B94;
}
