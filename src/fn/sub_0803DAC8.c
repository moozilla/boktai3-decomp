// MATCHING_CONCESSION: register-pinning
#include "scene_pool.h"

void ScenePool_SetSpeedControlSchedule(struct ScenePoolEntry *, s32, s32, s32);
static inline u8 PlayerHasFlag(u32 mask)
{
    if (gParticipantActors[0] && (gParticipantActors[0]->flags & mask))
        return 1;
    return 0;
}
void ScenePool_ScheduleSpeedControl(struct ScenePoolEntry *entry)
{
    struct ScenePoolEntry *base = entry;
    register struct ScenePoolEntry *arg asm("r0");
    s32 metric;
    s32 kind;
    if (PlayerHasFlag(1))
        goto medium;
    {
        s32 hi = 0x1e000;
        metric = base->direction;
        if (metric > hi)
            goto far;
        {
            s32 inside = 1;
            if (metric < -0x1e000)
                inside = 0;
            if (!inside)
                goto far;
        }
        if (metric > 0xf000)
            goto near;
        {
            s32 inside = 1;
            if (metric < -0xf000)
                inside = 0;
            if (!inside)
                goto near;
        }
        {
            {
                s32 random;
                register u32 *index asm("r0") = &gRandomIndex;
                u32 next = (*index + 1) & 0x3ff;
                *index = next;
                random = *(const u8 *)((next << 1) + (u32)gUnk_0203B400);
                if (random <= 31)
                    ScenePool_SetSpeedControlSchedule(base, 2, 2, 4);
                else if (random <= 63)
                    ScenePool_SetSpeedControlSchedule(base, 1, 2, 4);
                goto done;
            }
        near:
            if (metric > 0)
                goto medium;
            arg = base;
            kind = 1;
            goto schedule;
        }
    }
far: {
    register s32 value asm("r2") = metric;
    if (value <= 0)
        goto slow;
    if (value > 0x50000) {
        arg = base;
        kind = 4;
        goto schedule;
    }
}
    if (metric > 0x32000) {
        arg = base;
        kind = 3;
        goto schedule;
    }
medium:
    arg = base;
    kind = 2;
schedule:
    ScenePool_SetSpeedControlSchedule(arg, kind, 4, 8);
    goto done;
slow:
    ScenePool_SetSpeedControlSchedule(base, 1, 4, 8);
done:;
}
