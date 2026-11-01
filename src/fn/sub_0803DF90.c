#include "scene_pool.h"

#ifdef NONMATCHING
// fuzzy: 87.3%
// The random paths write their stores out (no Enter call: the constants are materialized at
// each store as in the ROM) and jump to done, so jump2 merges them into the direction>0 tail at
// the stateEntered store, as the ROM does. Left: the ROM keeps case 3's and the last
// direction's Enter tails unmerged and merges cases 0/1 and 2 into case 2's copy; here every
// tail merges into the direction>0 copy. jump2 takes the first jump in the target label's
// chain (last jump first), so the ROM's groups had different labels at jump2.
s32 ScenePool_SelectStateAtPositionLimit(struct ScenePoolEntry *);
s32 ScenePool_ResumeAfterIdle(void *, struct ScenePoolEntry *);
void ScenePool_UpdateSpeedPulse(struct ScenePoolActor *, struct ScenePoolEntry *,
                                const struct ScenePoolSpeedBounds *);
extern const struct ScenePoolSpeedBounds gUnk_0860588C[];
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
s32 ScenePool_StateTrackAdaptive(void *manager, struct ScenePoolActor *actor,
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
        goto done;
    if (ScenePool_ResumeAfterIdle(actor, entry))
        goto done;
    Or(0x10, &actor->stateFlags);
    ScenePool_UpdateSpeedPulse(actor, entry, gUnk_0860588C);
    if (entry->speedControlActive) {
        u32 *controls = &actor->controlFlags;
        u32 value = *controls;
        if (!(value & 0x200)) {
            s32 metric = entry->direction;
            s32 absolute = metric;
            if (metric < 0)
                absolute = -absolute;
            if (absolute > 0x1e000) {
                s32 lateral = entry->unknown5c;
                s32 magnitude = lateral;
                if (lateral < 0)
                    magnitude = -magnitude;
                if (magnitude > 0xa000) {
                    if (lateral > 0)
                        *controls = value | 0x80;
                    else
                        *controls = value | 0x40;
                }
            } else {
                s32 lateral = entry->unknown5c;
                s32 magnitude = lateral;
                if (lateral < 0)
                    magnitude = -magnitude;
                if (magnitude <= 0x9fff) {
                    if (lateral > 0)
                        *controls = value | 0x40;
                    else
                        *controls = value | 0x80;
                }
            }
        }
        history[0]++;
    } else
        history[0] = 0;
    flag = PlayerHasFlag(2);
    if (!flag && history[0] > 9) {
        switch (entry->kind) {
            case 0:
            case 1:
                Enter(entry, 5, flag); return 0;
            case 2:
                if (entry->mode == 0)
                    Enter(entry, 5, flag);
                else
                    Enter(entry, 7, flag); return 0;
            case 3:
                Enter(entry, 7, flag); break;
        }
    } else if (entry->stateFrame > 239) {
        flag = PlayerHasFlag(2);
        if (flag) {
            s32 sample;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            sample = *((u8 *)RANDOM_TABLE_ADDR + (gRandomIndex << 1));
            if (sample <= 127) {
                entry->state = 2;
                entry->stateFrame = 0;
                entry->stateEntered = 1;
                Memory_Clear(entry->history, 0x100); goto done;
            } else {
                entry->state = 0;
                entry->stateFrame = 0;
                entry->stateEntered = 1;
                Memory_Clear(entry->history, 0x100); goto done;
            }
        } else if (entry->direction > 0) {
            Enter(entry, 1, flag); goto done;
        } else {
            Enter(entry, 2, flag); return 0;
        }
    } else
        entry->stateFrame++;
done:
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", ScenePool_StateTrackAdaptive);
#endif
