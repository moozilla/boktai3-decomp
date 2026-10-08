#include "global.h"
struct T { u8 f[0x3a]; u16 a; u16 b; };
extern struct T gUnk_03005470;
void m4aSongNumStart(u32);
void sub_0822B524(void)
{
    if (gUnk_03005470.a != 0)
        m4aSongNumStart(gUnk_03005470.a);
    if (gUnk_03005470.b != 0)
        m4aSongNumStart(gUnk_03005470.b);
}
