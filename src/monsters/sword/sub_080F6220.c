#include "global.h"

struct TargetMotionVector {
    u32 x : 16;
    s32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
struct TargetMotionOwner {
    u8 unknown0[0xD56];
    u16 palette;
};
struct TargetMotionExit {
    u8 unknown0[0xC];
    void *animation;
    u8 unknown10[0x3E];
    u16 flags4E;
    u8 unknown50[0x1C];
    struct TargetMotionVector position;
    u8 unknown74[0x2E];
    u16 flagsA2;
    u8 unknownA4[0x75];
    u8 flags119;
    u8 unknown11A[4], state11E, state11F;
    u8 unknown120[0xC], elapsed, duration, reset;
    u8 unknown12F[0x31];
    struct TargetMotionOwner *owner;
};
void AnimData_SelectPaletteBank(void *, u32);
void Sound_PlaySong(u32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);
static inline u32 TargetMotion_ClearFlag(u32 flags)
{
    u32 m = ~1;
    m &= flags;
    return m;
}
static inline void TargetMotion_SetFlag(struct TargetMotionExit *p, u8 flag)
{
    p->flags119 |= flag;
}
static inline u8 TargetMotion_TakeExitFlag(struct TargetMotionExit *p)
{
    u32 mask = 1;
    if (mask & p->flags119) {
        p->flags119 = TargetMotion_ClearFlag(p->flags119);
        p->reset = 0;
        return 1;
    }
    return 0;
}
void TargetMotion_UpdateFlashingExit(struct TargetMotionExit *p)
{
    struct TargetMotionOwner *owner = p->owner;
    struct TargetMotionVector position, size;
    if (TargetMotion_TakeExitFlag(p)) {
        p->flags4E |= 4;
        p->flagsA2 |= 4;
        p->elapsed = 0;
        p->duration = 20;
    }
    if (Sprite_BlinkUntilExpired(p, p->elapsed, 10, 30)) {
        AnimData_SelectPaletteBank(p->animation, owner->palette);
        {
            u8 state = 5;
            p->state11E = 0;
            p->state11F = state;
        }
        TargetMotion_SetFlag(p, 1);
    } else {
        if (!(p->elapsed & 7)) {
            position = p->position;
            position.x += 100;
            position.y += 100;
            position.z += 100;
            size.x = 80;
            size.y = 60;
            size.z = 40;
            SparkParticle_Spawn(&position, 1, &size, 0, 0, 14, 2, 2);
            Sound_PlaySong(366);
        }
        p->elapsed += 2;
    }
}
