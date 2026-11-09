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
static inline s32 Within(s32 value, s32 limit)
{
    if (value > limit)
        return 0;
    {
        s32 inside = 1;
        if (value < -limit)
            inside = 0;
        return inside;
    }
}
static inline s32 RandomByte(const u16 *table)
{
    s32 value;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    value = table[gRandomIndex];
    return value & 0xff;
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
    if (Within(entry->direction, 0x28000)) {
        metric = entry->direction;
        if (metric > 0x1e000)
            actor->controlFlags |= 0x10;
        else if (metric < -0x1e000)
            actor->controlFlags |= 0x20;
        history[0]++;
        history[1]--;
    } else {
        metric = entry->direction;
        if (metric > 0x78000)
            actor->controlFlags |= 0x300;
        else if (metric < -0x14000)
            actor->controlFlags |= 0x20;
        else
            actor->controlFlags |= 0x110;
        history[0] = 0;
        history[1]++;
    }
    /* The history pointer is reused for the random table: a pseudo first set to a non-constant gets
     * no REG_EQUIV, so the table keeps full priority (README fact 20). */
    history = (u32 *)gRandomTable;
    if (RandomByte((const u16 *)history) <= 31) {
        s32 z = actor->position.z >> 12;
        if (z <= 18)
            ScenePool_SetLateralControlSchedule(entry, 1, 3, 6);
        else if (z > 47)
            ScenePool_SetLateralControlSchedule(entry, -1, 3, 6);
        else if (RandomByte((const u16 *)history) <= 127)
            ScenePool_SetLateralControlSchedule(entry, 1, 3, 6);
        else
            ScenePool_SetLateralControlSchedule(entry, -1, 3, 6);
    }
    if (ScenePool_SelectStateAtPositionLimit(entry))
        return 0;
    if (ScenePool_ResumeAfterIdle(actor, entry))
        return 0;
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
    return 0;
}
