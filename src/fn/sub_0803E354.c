// MATCHING_CONCESSION: register-pinning
#include "scene_pool.h"
void ScenePool_SetSpeedControlSchedule(struct ScenePoolEntry *, s32, s32, s32);
void ScenePool_SetLateralControlSchedule(struct ScenePoolEntry *, s32, s32, s32);
s32 ScenePool_SelectStateAtPositionLimit(struct ScenePoolEntry *);
void ScenePool_ApplyScheduledSpeedControl(struct ScenePoolActor *, struct ScenePoolEntry *);
void ScenePool_UpdateLateralControlPulse(struct ScenePoolActor *, struct ScenePoolEntry *);
void ScenePool_ScheduleSpeedControl(struct ScenePoolEntry *);
static inline void Or(u32 mask, u32 *flags)
{
    *flags |= mask;
}
s32 ScenePool_StateScheduleMotion(void *manager, struct ScenePoolActor *actor,
                                  struct ScenePoolEntry *entry)
{
    u32 *history = entry->history;
    u32 zero;
    if (entry->stateEntered) {
        u32 initialZero = 0;
        entry->stateEntered = initialZero;
        ScenePool_SetSpeedControlSchedule(entry, 0, 30, 40);
        entry->turnStep = 0x200;
        history[0] = initialZero;
    }
    if (ScenePool_SelectStateAtPositionLimit(entry))
        goto done;
    Or(0x10, &actor->stateFlags);
    if (!entry->delayA) {
        ScenePool_ScheduleSpeedControl(entry);
        {
            const u16 *table = gUnk_0203B400;
            u32 *index = &gRandomIndex;
            u32 next;
            u32 indexMask;
            register u32 byteMask asm("r4");
            const u8 *randomAddress;
            s32 random;
            asm("" : "+r"(table), "+r"(index));
            next = *index + 1;
            indexMask = 0x3ff;
            next &= indexMask;
            *index = next;
            randomAddress = (const u8 *)((next << 1) + (u32)table);
            byteMask = 255;
            random = *randomAddress;
            if (random <= 31) {
                s32 z = actor->position.z >> 12;
                if (z <= 18)
                    ScenePool_SetLateralControlSchedule(entry, 1, 3, 6);
                else if (z > 47)
                    ScenePool_SetLateralControlSchedule(entry, -1, 3, 6);
                else {
                    u32 value;
                    u32 second = (next + 1) & indexMask;
                    *index = second;
                    value = table[second];
                    random = value & byteMask;
                    if (random <= 127)
                        ScenePool_SetLateralControlSchedule(entry, 1, 3, 6);
                    else
                        ScenePool_SetLateralControlSchedule(entry, -1, 3, 6);
                }
            }
        }
    } else
        entry->delayA--;
    ScenePool_ApplyScheduledSpeedControl(actor, entry);
    ScenePool_UpdateLateralControlPulse(actor, entry);
    if ((u32)entry->direction + 0x4ffff <= 0x9fffe)
        history[0]++;
    else
        history[0] = 0;
    if (history[0] > 9) {
        s32 loaded = entry->direction;
        u32 lower = 0xfffebfff;
        u32 window = (u32)loaded + lower;
        u32 upper = 0x81ffe;
        s32 metric = loaded;
        if (window <= upper) {
            entry->state = 5;
            zero = 0;
            goto reset;
        }
        if (metric > -0x78000 && metric < -0x14000) {
            entry->state = 6;
            zero = 0;
            asm("" : "+r"(zero));
            goto reset;
        }
        {
            u32 narrow = (u32)metric + 0x13fff;
            if (narrow <= 0x27ffe) {
                entry->state = 4;
                zero = 0;
                asm("" : "+r"(zero));
                goto reset;
            }
        }
    }
    if (entry->stateFrame <= 239) {
        entry->stateFrame++;
        goto done;
    }
    zero = 0;
    entry->state = zero;
reset:
    entry->stateFrame = zero;
    entry->stateEntered = 1;
    {
        u32 *arg = entry->history;
        Memory_Clear(arg, 0x100);
    }
done:
    return 0;
}
