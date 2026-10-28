// MATCHING_CONCESSION: register-pinning
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
    u8 *entered;
    register u32 value asm("r0");
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
        asm volatile("");
        goto done;
    }
    if (entry->stateFrame > 239) {
        flag = PlayerHasFlag(2);
        if (flag) {
            s32 random;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            {
                u32 offset = gRandomIndex << 1;
                register const u8 *table asm("r3");
                table = (const u8 *)gRandomTable;
                random = *(const u8 *)(offset + (u32)table);
            }
            if (random <= 127) {
                entry->state = 2;
                entry->stateFrame = 0;
                entered = &entry->stateEntered;
                value = 1;
                goto sharedEntry;
            } else {
                entry->state = 0;
                entry->stateFrame = 0;
                entered = &entry->stateEntered;
                value = 1;
                asm("" : "+r"(entered), "+r"(value));
                goto sharedEntry;
            }
        } else if (entry->direction > 0) {
            value = 1;
            entry->state = value;
            entry->stateFrame = flag;
            entered = &entry->stateEntered;
        sharedEntry:
            *entered = value;
            {
                u32 *arg = entry->history;
                Memory_Clear(arg, 0x100);
            }
            goto done;
        } else {
            Enter(entry, 2, flag);
            asm volatile("");
        }
    } else
        entry->stateFrame++;
done:
    return 0;
}
