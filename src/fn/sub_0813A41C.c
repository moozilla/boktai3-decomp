#define SpritePairEntry_CreateEffect sub_0813A41C
#include "global.h"
struct EffectPackedCoordinate {
    u32 lo : 16;
    u32 hi : 16;
};
struct EffectOffset {
    struct EffectPackedCoordinate a, b;
};
struct SpritePairEffectEntry {
    u8 pad0[0xa0];
    u32 position;
    u8 padA4[0x20];
    u32 *height;
};
void Position_GetHeightAdjustedCoordinates(u32, u32, void *);
void sub_0811B278(void *, void *, struct EffectOffset *, struct EffectOffset *, u32, u32, u32, u32,
                  u32, u32, u32);
void sub_0813A41C(struct SpritePairEffectEntry *entry, u32 a, u32 b, u32 c)
{
    u32 position[2];
    struct EffectOffset first, second;
    u32 height = *entry->height - 0x20000;
    Position_GetHeightAdjustedCoordinates(entry->position, height, position);
    first.a.lo = 10;
    first.a.hi = 8;
    first.b.lo = 23;
    second.a.lo = 10;
    second.a.hi = first.a.hi;
    second.b.lo = -21;
    sub_0811B278((u8 *)entry + 4, position, &first, &second, 12, c, 5, b, 20, a, (u32)entry);
}
