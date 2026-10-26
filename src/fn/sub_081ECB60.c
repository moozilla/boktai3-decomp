#define gVariantRandomCursor gEffectRandomTableIndex
#define gVariantRandomTable  gUnk_086141BC
#include "global.h"
#include "ram.h"
struct ScreenVariant {
    u8 unknown0[0x288];
    u32 variant;
    u8 unknown28C[0x319];
    u8 width;
};
extern const u16 gUnk_086141BC[];
void ScreenLayers_SelectRandomVariant(struct ScreenVariant *screen)
{
    u32 mask;
    switch (screen->width) {
        case 1:
            mask = 1;
            break;
        case 2:
            mask = 3;
            break;
        case 3:
            mask = 7;
            break;
        case 0:
        default:
            mask = 0;
            break;
    }
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3FF;
    {
        u32 value = gUnk_086141BC[gEffectRandomTableIndex];
        screen->variant = value & mask;
    }
}
