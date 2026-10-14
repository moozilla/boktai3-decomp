// MATCHING_CONCESSION: register-pinning
#include "scene_pool.h"
static inline u32 *Prepare(struct ScenePoolEntry *entry, u32 state)
{
    entry->state = state;
    entry->stateFrame = 0;
    entry->stateEntered = 1;
    return entry->history;
}
s32 ScenePool_StateLateralWeave(void *manager, struct ScenePoolActor *actor,
                                struct ScenePoolEntry *entry)
{
    if (entry->stateEntered) {
        entry->stateEntered = 0;
        if ((s32)entry->unknown5c > 0)
            entry->settingB = 1;
        else
            entry->settingB = -1;
        entry->turnStep = 0x180;
    }
    if (entry->stateFrame <= 7) {
        if (entry->settingB <= 0)
            actor->controlFlags |= 0x80;
        else
            actor->controlFlags |= 0x40;
    } else if (entry->stateFrame > 24) {
        if (entry->settingB > 0)
            actor->controlFlags |= 0x80;
        else
            actor->controlFlags |= 0x40;
    }
    if (entry->stateFrame > 59) {
        s32 kind;
        u32 *arg;
        entry->turnStep = 0x200;
        kind = entry->kind;
        switch (kind) {
            case 0:
            case 1:
                arg = Prepare(entry, 0);
                goto clear;
            case 2:
                if (entry->direction > 0)
                    goto first;
                arg = Prepare(entry, kind);
                asm("" : "+r"(arg));
                goto clear;
            case 3:
                if (entry->direction <= 0)
                    goto second;
            first: {
                u32 one = 1;
                register u8 *entered asm("r0");
                entry->state = one;
                entry->stateFrame = 0;
                entered = &entry->stateEntered;
                *entered = one;
                arg = (u32 *)(entered + 8);
            }
            clear:
                Memory_Clear(arg, 0x100);
                goto done;
            second:
                Memory_Clear(Prepare(entry, 2), 0x100);
                asm volatile("");
                goto done;
        }
    } else
        entry->stateFrame++;
done:
    return 0;
}
