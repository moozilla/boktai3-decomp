#include "global.h"
#include "ram.h"

struct S {
    u8 f[0xd0];
    u32 a;
};
void TextWindow_Destroy(u32);
void Bg_ClearTilemap(u32);
u32 WirelessTradeMenu_Destroy(struct S *p)
{
    TextWindow_Destroy(p->a);
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(2);
    return gWirelessTradeMenu = 0;
}
