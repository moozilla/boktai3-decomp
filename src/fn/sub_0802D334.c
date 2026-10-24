#include "global.h"
struct ActorActionQueue {
    u8 pad0[0x11C];
    const void *action;
    u8 pad120[0x12A - 0x120];
    u16 current;
    u8 pad12C;
    u8 entered;
    u16 pending;
    u8 pad130[0x134 - 0x130];
    u32 timer;
};
extern const void *const gUnk_08605050[];
// confidence=low: starts the pending action (from gUnk_08605050) if there is one.
s32 sub_0802D334(struct ActorActionQueue *p)
{
    s32 started;
    if (p->pending) {
        const void *const *table = gUnk_08605050;
        u16 id = p->pending;
        const void *action;
        action = table[id];
        p->action = action;
        p->current = id;
        p->entered = 1;
        p->timer = 0;
        started = 1;
    } else
        started = 0;
    return started;
}
