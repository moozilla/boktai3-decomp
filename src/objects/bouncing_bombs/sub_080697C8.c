#include "global.h"
#include "engine/engine.h"

#include "gba/macro.h"
#include "gba/syscall.h"
struct PackedVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
u8 *BouncingBomb_Spawn(struct PackedVec *, struct PackedVec *, struct PackedVec *, s32, s32, s32,
                         s32);
// The plain conditional expression keeps each of the six expansions short before combine,
// which keeps &pos ahead of b in global allocation (r8 and sb, as in the ROM).
static inline s32 optionalValue(void)
{
    return Script_GetPc() ? Script_GetValue() : 0;
}

static inline void BouncingBomb_ClearVector(struct PackedVec *vector)
{
    CpuFill32(0, vector, sizeof(*vector));
}
// confidence=low: spawns a bouncing object from script keywords p (position), a (shot,
// category), v (velocity), l and n, and sets flag 1 at 0x134.
#define BouncingBomb_SpawnFromScript sub_080697C8
void BouncingBomb_SpawnFromScript(void)
{
    struct PackedVec pos;
    struct PackedVec vel;
    struct PackedVec zero;
    s32 a;
    s32 l;
    s32 n;
    s32 b;
    u8 *object;
    b = 0;
    a = 0;
    if (Script_SeekToKeyword('p')) {
        pos.x = optionalValue();
        pos.y = optionalValue();
        pos.z = optionalValue();
    }
    if (Script_SeekToKeyword('a')) {
        a = optionalValue();
        if (Script_GetPc())
            b = Script_GetValue();
        else
            b = 0;
    }
    if (Script_SeekToKeyword('v')) {
        vel.x = optionalValue();
        vel.y = optionalValue();
        vel.z = optionalValue();
    } else {
        vel.x = 0;
        vel.y = 0;
        vel.z = 0;
    }
    l = Script_GetKeywordValue('l', 0);
    n = Script_GetKeywordValue('n', 0);
    BouncingBomb_ClearVector(&zero);
    object = BouncingBomb_Spawn(&pos, &vel, &zero, a, b, l, n);
    if (object) {
        u16 *flags = (u16 *)(object + 0x134);
        u32 value = *flags;
        u32 mask = 1;
        mask |= value;
        *flags = mask;
    }
}
