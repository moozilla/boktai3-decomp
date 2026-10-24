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
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// The ROM loads the table entry before the store address and keeps 0x11C in r4.
s32 sub_0802D334(struct ActorActionQueue *p)
{
    s32 started;
    u16 id = p->pending;
    typeof(gUnk_08605050[id]) t1;
    if (id) {
        t1 = gUnk_08605050[id];
        p->action = t1;
        p->current = id;
        p->entered = 1;
        do {
            p->timer = 0;
        } while (0);
        started = 1;
    } else
        started = 0;
    return started;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0802D334);
#endif
