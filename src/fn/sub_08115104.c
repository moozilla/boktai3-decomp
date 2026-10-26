#define gEffectUpdateCallbacks gUnk_086106B4
#include "global.h"
#include "render.h"
#include "engine.h"
#include "ram.h"
struct EffectUpdateState {
    u16 flags;
    u8 type, pad13;
    u32 value;
    u32 active;
    u8 padding[16];
};
struct EffectUpdateEntry {
    u8 payload[0x10];
    u16 flags;
    u8 type, pad13;
    u32 value;
    u32 active;
};
struct EffectUpdatePool {
    u8 pad0[0x720];
    struct EffectUpdateEntry entries[32];
};
extern void *gUnk_086106B4[];
s32 sub_08249240(void *, void *, void *);
s32 EffectPool_UpdateEntries(struct EffectUpdatePool *pool)
{
    u32 mask = 0x200;
    struct EffectUpdateState *state = (struct EffectUpdateState *)((u8 *)pool + 0x730);
    struct EffectUpdateEntry *entry = pool->entries;
    s32 i = 31;
    do {
        if (state->active) {
            if (((gEngineFlags | gScenePauseFlags) & mask) ||
                ((gEngineFlags & 0x800) && (gEnginePauseFlags & 7))) {
                if (state->flags & 1)
                    goto next;
            }
            sub_08249240(pool, entry, gUnk_086106B4[state->type]);
        }
    next:
        state++;
        entry++;
    } while (--i >= 0);
    return 0;
}
