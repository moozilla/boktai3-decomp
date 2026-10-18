#define ActorEffect_EmitOffsetPair sub_0811F29C
// Tentative descriptive names; inferred from static field accesses and callees.
#include "global.h"
struct EffectHalfPair {
    u32 lo : 16;
    s32 hi : 16;
};
struct EffectExtent {
    struct EffectHalfPair x, y;
};
struct EffectSource {
    u8 pad0[0xa];
    u16 id;
    u8 padC[8];
    u8 origin[1];
};
void Character_GetHeightAdjustedPosition(void *, s16 *);
void sub_0811B278(void *, void *, struct EffectExtent *, struct EffectExtent *, u32, u32, u32, u32,
                  u32, u32, u32);
void sub_0811F29C(void *owner, void *unused, struct EffectSource *source, u32 arg, u32 kind)
{
    s16 position[4];
    struct EffectExtent extent, offset;
    Character_GetHeightAdjustedPosition(owner, position);
    extent.x.lo = 24;
    extent.x.hi = 2;
    extent.y.lo = 12;
    offset.x.lo = 32;
    offset.x.hi = -extent.x.hi;
    offset.y.lo = -extent.y.lo;
    sub_0811B278(source->origin, position, &extent, &offset, 12, kind, 5, source->id, 30, arg,
                 (u32)owner);
}
