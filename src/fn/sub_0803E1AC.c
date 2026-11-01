#include "scene_pool.h"
s32 ScenePool_SelectStateAtPositionLimit(struct ScenePoolEntry *);
s32 ScenePool_ResumeAfterIdle(void *, struct ScenePoolEntry *);
void ScenePool_UpdateSpeedPulse(struct ScenePoolActor *, struct ScenePoolEntry *,
                                const struct ScenePoolSpeedBounds *);
extern const struct ScenePoolSpeedBounds gUnk_086058A0[];
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
static inline void Enter(struct ScenePoolEntry *entry, u32 state, u32 zero)
{
    entry->state = state;
    entry->stateFrame = zero;
    entry->stateEntered = 1;
    Memory_Clear(entry->history, 0x100);
}
s32 ScenePool_StateTrackFast(void *manager, struct ScenePoolActor *actor,
                             struct ScenePoolEntry *entry)
{
    u32 *history = entry->history;
    u32 flag;
    if (entry->stateEntered) {
        entry->stateEntered = 0;
        history[0] = 0;
        entry->speedControlActive = 0;
        entry->turnStep = 0x200;
    }
    if (ScenePool_SelectStateAtPositionLimit(entry))
        return 0;
    if (ScenePool_ResumeAfterIdle(actor, entry))
        return 0;
    Or(0x10, &actor->stateFlags);
    ScenePool_UpdateSpeedPulse(actor, entry, gUnk_086058A0);
    if (entry->speedControlActive) {
        u32 *controls = &actor->controlFlags;
        u32 value = *controls;
        if (!(value & 0x200)) {
            s32 metric = entry->unknown5c;
            s32 absolute = metric;
            if (metric < 0)
                absolute = -absolute;
            if (absolute > 0xa000) {
                if (metric > 0)
                    *controls = value | 0x80;
                else
                    *controls = value | 0x40;
            }
        }
        history[0]++;
    } else
        history[0] = 0;
    flag = PlayerHasFlag(2);
    if (!flag && history[0] > 9) {
        Enter(entry, 6, flag);
        return 0;
    }
    if (entry->stateFrame > 239) {
        flag = PlayerHasFlag(2);
        if (flag) {
            s32 sample;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            sample = *((u8 *)RANDOM_TABLE_ADDR + (gRandomIndex << 1));
            if (sample <= 127) {
                entry->state = 2;
                entry->stateFrame = 0;
                entry->stateEntered = 1;
                Memory_Clear(entry->history, 0x100);
            } else {
                entry->state = 0;
                entry->stateFrame = 0;
                entry->stateEntered = 1;
                Memory_Clear(entry->history, 0x100);
            }
        } else if (entry->direction > 0) {
            Enter(entry, 1, flag);
        } else {
            Enter(entry, 2, flag);
        }
        return 0;
    }
    entry->stateFrame++;
    return 0;
}
