#include "scene_pool.h"

#ifdef NONMATCHING
// twin.py transplant from a matched near-clone
// fix.py best version (reprinted C)
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
    if (gUnk_020004F0[0] && (gUnk_020004F0[0]->flags & mask))
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
static inline void Enter_1(struct ScenePoolEntry *entry, u32 zero)
{
    entry->state = 7;
    entry->stateFrame = zero;
    {
        typeof(entry->stateEntered) *q1 = &entry->stateEntered;
        *q1 = 1;
    }
    Memory_Clear(entry->history, 0x100);
}

s32 ScenePool_StateTrackAdaptive(void *manager, struct ScenePoolActor *actor,
                                 struct ScenePoolEntry *entry)
{
    u32 *history = entry->history;
    u32 flag;
    u8 *entered;
    u32 value;
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
    if ((!flag) && (history[0] > 9)) {
        u32 state;
        switch (entry->kind) {
            case 0:

            case 1:
                state = 5;
                break;

            case 2:
                if (entry->mode != 0)
                    state = 7;
                else
                    state = 5;
                break;

            case 3:
                Enter_1(entry, flag);
                goto done;

            default:
                goto done;
        }

        Enter(entry, state, flag);
        goto done;
    }
    if (entry->stateFrame > 239) {
        flag = PlayerHasFlag(2);
        if (flag) {
            s32 random;
            gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
            {
                u32 offset = gUnk_03005308 << 1;
                register const u8 *table asm("r3");
                table = (const u8 *)gUnk_0203B400;
                random = *((const u8 *)(offset + ((u32)table)));
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

#else
INCLUDE_ASM("asm/nonmatching", ScenePool_StateTrackAdaptive);
#endif
