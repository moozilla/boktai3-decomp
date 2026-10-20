#include "global.h"

#define SceneNpc_UpdateBlinkWithCallback sub_081CF518

struct Npc081CF518 {
    u8 pad0[7];
    u8 done, changed, entered;
    u8 padA[4];
    u16 flags;
    u8 pad10[4];
    s32 timer;
    u8 phase;
    u8 pad19[0x2E4 - 0x19];
    u8 (*callback)(struct Npc081CF518 *);
};

void SceneNpc_UpdateBlinkWithCallback(void *ctx, struct Npc081CF518 *npc)
{
    if (npc->entered) {
        s32 zero = 0;
        npc->entered = zero;
        npc->done = zero;
        npc->timer = 16;
    }
    if (npc->callback(npc) != 1) {
        u8 phase = npc->phase;
        if (phase & 1) {
            u32 mask = 0x100;
            npc->flags = mask | npc->flags;
        }
        if (npc->timer) {
            if (--npc->timer == 0) {
                npc->phase = (phase + 1) & 1;
                npc->timer = 16;
            }
        }
    }
}
