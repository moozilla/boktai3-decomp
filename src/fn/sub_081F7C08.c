#include "global.h"
/* SceneActor_UpdateDirectionalTrigger_081F7C08: same as SceneActor_UpdateDirectionalTrigger but sets the owner's +0x184 flag instead of consuming a trigger (provisional name) */
#define SceneActor_UpdateDirectionalTrigger_081F7C08 sub_081F7C08

u32 Trigger_ConsumePending(void *latch, u32 expected);

// Tentative scene-actor layout; state semantics remain provisional.
struct SceneDirectionalActor {
    u8 pad00[0x51];
    u8 direction;
    u8 pad52[0xA8 - 0x52];
    u8 direction4;
    u8 direction8;
    u8 state;
    u8 previousState;
    u8 padAC[3];
    u8 pending;
    u8 entered;
    u8 padB1[0xC4 - 0xB1];
    u32 timer;
    u8 padC8[4];
    u8 requestedDirection;
};

static inline u8 TakeEntry(struct SceneDirectionalActor *p)
{
    if (p->entered) {
        p->entered = 0;
        p->pending = 0;
        return TRUE;
    }
    return FALSE;
}

static inline u8 SelectState(s32 value)
{
    if (value == 0)
        value = 3;
    else
        value = 4;
    return value;
}

void SceneActor_UpdateDirectionalTrigger_081F7C08(struct SceneDirectionalActor *p)
{
    u8 *q = (u8 *)p;
    p->direction = p->requestedDirection;
    {
        s32 direction = (p->direction + 0x60) >> 6;
        u32 mask = 3;
        mask &= direction;
        p->direction4 = mask;
    }
    {
        s32 direction = (p->direction + 0x70) >> 5;
        u32 mask = 7;
        mask &= direction;
        p->direction8 = mask;
    }
    if (TakeEntry(p)) {
        u8 state = SelectState(*(s16 *)(q + 0xE58));
        u8 *dest = &p->state;
        *dest = state;
    }
    if (p->pending) {
        if (q[0xAB] != 3) {
            *(u32 *)(q + 0x184) = 1;
            return;
        }
        {
            u32 state = 4;
            q[0xAA] = state;
        }
    }
    p->timer++;
}
