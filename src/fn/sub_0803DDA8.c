// MATCHING_CONCESSION: register-pinning
#include "scene_pool.h"

void ScenePool_SetLateralControlSchedule(struct ScenePoolEntry *, s32, s32, s32);
s32 ScenePool_SelectStateAtPositionLimit(struct ScenePoolEntry *);
s32 ScenePool_ResumeAfterIdle(struct ScenePoolActor *, struct ScenePoolEntry *);
s32 ScenePool_SelectInitialDirection(struct ScenePoolActor *, struct ScenePoolEntry *);
s32 ScenePool_SelectRepeatOrExit(struct ScenePoolActor *, struct ScenePoolEntry *);
s32 ScenePool_SelectRepeatedDirection(struct ScenePoolActor *, struct ScenePoolEntry *);
s32 ScenePool_SelectDirectionOrRepeat(struct ScenePoolActor *, struct ScenePoolEntry *);
static inline void Or(u32 mask, u32 *flags)
{
    *flags |= mask;
}
static inline u8 PlayerHasFlag(u32 mask)
{
    if (gParticipantActors[0] && (gParticipantActors[0]->flags & mask))
        return 1;
    return 0;
}
s32 ScenePool_StateCruise(void *manager, struct ScenePoolActor *actor, struct ScenePoolEntry *entry)
{
    u32 *history = entry->history;
    s32 metric;
    if (entry->stateEntered) {
        entry->stateEntered = 0;
        history[0] = 0;
        history[1] = 0;
        entry->turnStep = 0x200;
        {
            u32 *flags = &actor->motionFlags;
            u32 mask;
            mask = ~1;
            *flags &= mask;
        }
    }
    {
        s32 hi = 0x28000;
        metric = entry->direction;
        if (metric > hi)
            goto outside;
        {
            s32 inside = 1;
            if (metric < -0x28000)
                inside = 0;
            if (!inside)
                goto outside;
        }
        {
            if (metric > 0x1e000)
                actor->controlFlags |= 0x10;
            else if (metric < -0x1e000)
                actor->controlFlags |= 0x20;
            history[0]++;
            history[1]--;
            goto rangeDone;
        }
    outside: {
        if (metric > 0x78000)
            actor->controlFlags |= 0x300;
        else if (metric < -0x14000)
            actor->controlFlags |= 0x20;
        else
            actor->controlFlags |= 0x110;
        history[0] = 0;
        history[1]++;
    }
    rangeDone:;
    }
    {
        const u16 *table = gUnk_0203B400;
        u32 *index = &gRandomIndex;
        u32 next = *index + 1;
        u32 indexMask = 0x3ff;
        register u32 byteMask asm("r6");
        const u8 *randomAddress;
        s32 random;
        asm("" : "+r"(table), "+r"(index));
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
    if (ScenePool_SelectStateAtPositionLimit(entry))
        goto done;
    if (ScenePool_ResumeAfterIdle(actor, entry))
        goto done;
    Or(0x10, &actor->stateFlags);
    if (!PlayerHasFlag(2)) {
        switch (entry->kind) {
            case 0:
                ScenePool_SelectInitialDirection(actor, entry);
                break;
            case 1:
                ScenePool_SelectRepeatOrExit(actor, entry);
                break;
            case 2:
                ScenePool_SelectRepeatedDirection(actor, entry);
                break;
            case 3:
                ScenePool_SelectDirectionOrRepeat(actor, entry);
                break;
        }
    }
    entry->stateFrame++;
done:
    return 0;
}
