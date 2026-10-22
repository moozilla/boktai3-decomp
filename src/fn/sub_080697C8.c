#include "global.h"
#include "engine.h"

#ifdef NONMATCHING
// fuzzy: 86.4%
// Remaining: the early zp pointer only exists to give z the lowest stack slot (as in the ROM);
// it costs a register and a spill for a. Replace it with whatever makes z first.
struct PackedVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
u8 *BouncingObject_Spawn(struct PackedVec *, struct PackedVec *, struct PackedVec *, s32, s32, s32,
                         s32);
static inline s32 optionalValue(void)
{
    s32 value;
    if (Script_GetPc())
        value = Script_GetValue();
    else
        value = 0;
    return value;
}
// confidence=low: spawns a bouncing object from script keywords p (position), a (shot,
// category), v (velocity), l and n, and sets flag 1 at 0x134.
void sub_080697C8(void)
{
    struct PackedVec pos;
    struct PackedVec vel;
    struct PackedVec zero;
    s32 a, b, l, n;
    u8 *object;
    u32 z;
    u32 *zp = &z;
    b = 0;
    a = 0;
    if (Script_SeekToKeyword('p')) {
        pos.x = optionalValue();
        pos.y = optionalValue();
        pos.z = optionalValue();
    }
    if (Script_SeekToKeyword('a')) {
        a = optionalValue();
        b = optionalValue();
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
    *zp = 0;
    CpuSet(zp, &zero, 0x05000002);
    object = BouncingObject_Spawn(&pos, &vel, &zero, a, b, l, n);
    if (object)
        *(u16 *)(object + 0x134) |= 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080697C8);
#endif
