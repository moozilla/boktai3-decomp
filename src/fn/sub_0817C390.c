#include "global.h"
struct SceneActorFlagWait {
    u8 pad0[0x9c];
    u32 flags;
    u8 padA0[0xa];
    u8 state;
    u8 padAB;
    u8 phase0;
    u8 phase1;
    u8 phase2;
    u8 phase;
    u8 pending;
    u8 padB1[0x13];
    u32 timer;
};
static inline u8 consume(struct SceneActorFlagWait *p)
{
    if (p->pending) {
        p->pending = 0;
        p->phase = 0;
        return 1;
    }
    return 0;
}
void SceneActorState_WaitForFlagClear(struct SceneActorFlagWait *p)
{
    if (consume(p)) {
        u32 state = 12;
        p->state = state;
    } else {
        u32 one = 1;
        if (!(p->flags & one)) {
            p->phase0 = 0;
            p->phase1 = 0;
            p->phase2 = 0;
            p->timer = 0;
            p->pending = one;
            p->state = one;
        }
    }
    p->timer++;
}
