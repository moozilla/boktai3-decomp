// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"

#include "gba/macro.h"
#include "gba/syscall.h"
struct PackedVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
u8 *BouncingObject_Spawn(struct PackedVec *, struct PackedVec *, struct PackedVec *, s32, s32, s32,
                         s32);
static inline s32 optionalValue(void)
{
    s32 value;
    value = (Script_GetPc()) ? (Script_GetValue()) : (0);
    return value;
}

// confidence=low: spawns a bouncing object from script keywords p (position), a (shot,
// category), v (velocity), l and n, and sets flag 1 at 0x134.
#define BouncingObject_SpawnFromScript sub_080697C8
void BouncingObject_SpawnFromScript(void)
{
    struct PackedVec pos;
    struct PackedVec vel;
    struct PackedVec zero;
    s32 a;
    s32 l;
    s32 n;
    register s32 b asm("r9");
    struct PackedVec *out;
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
        else {
            register u32 value asm("r1") = 0;
            asm("" : "+r"(value));
            b = value;
        }
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
    {
        vu32 fill;
        out = &zero;
        fill = 0;
        {
            const void *source = (const void *)(&fill);
            register void *dest asm("r1") = out;
            asm("" : : "r"(source), "r"(dest));
            CpuSet(source, dest, 0x05000002);
        }
    }
    object = BouncingObject_Spawn(&pos, &vel, out, a, b, l, n);
    if (object) {
        u16 *flags = (u16 *)(object + 0x134);
        u32 value = *flags;
        u32 mask = 1;
        mask |= value;
        *flags = mask;
    }
}
