#define ScreenLayers_SelectRandomVariant sub_081ECB60
#define gVariantRandomCursor gUnk_03005304
#define gVariantRandomTable gUnk_086141BC
#include "global.h"
struct ScreenVariant { u8 unknown0[0x288]; u32 variant; u8 unknown28C[0x319]; u8 width; };
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
void sub_081ECB60(struct ScreenVariant *screen)
{
    u32 mask;
    switch (screen->width) {
    case 1: mask = 1; break;
    case 2: mask = 3; break;
    case 3: mask = 7; break;
    case 0:
    default: mask = 0; break;
    }
    gUnk_03005304 = (gUnk_03005304 + 1) & 0x3FF;
    { u32 value = gUnk_086141BC[gUnk_03005304]; screen->variant = value & mask; }
}
