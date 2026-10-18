#include "global.h"
struct DirectionalEffectPosition {
    u16 x, z, y, unknown6;
};
struct DirectionalEffect {
    u8 unknown0[0x7fc];
    u32 flags;
    u8 unknown800[2], palette;
    u8 unknown803[0x15];
    struct DirectionalEffectPosition position;
    u8 unknown820[0x40];
    u8 active;
    u8 unknown861[0x3a9];
    u16 paletteValue;
};
void DirectionalEffect_PlaceSprite(struct DirectionalEffect *p, s32 unused,
                                   struct DirectionalEffectPosition *position)
{
    p->flags &= ~1;
    p->position = *position;
    p->position.z += 230;
    p->palette = p->paletteValue;
    p->active = 1;
}
