#include "global.h"
#include "engine.h"
#include "ram.h"
s32 Mod(s32, s32);
s32 Path_GetPointPositionById(u16 *, u32, u32);
struct ActorVariationState {
    u8 unknown0[0x4E3];
    u8 variant, detail;
};
void ActorVariation_SelectRandomPair(struct ActorVariationState *p, u16 *out)
{
    u32 table = (u32)gRandomTable;
    u32 index = (gRandomIndex + 1) & 1023;
    s32 sample = *(u16 *)(table + index * 2);
    p->variant = (sample >> 8) & 3;
    gRandomIndex = (index + 1) & 1023;
    p->detail = Mod(*(u16 *)(table + gRandomIndex * 2), 5);
    Path_GetPointPositionById(out, p->variant, p->detail);
}
