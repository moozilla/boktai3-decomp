#include "global.h"
/* Tentative Hresvelgr layout and behavior names, inferred from static consumers. */
typedef struct HresvelgrPosition {
    s16 x, y, z, pad;
} HresvelgrPosition;
typedef struct HresvelgrActor {
    u8 pad0[0x54];
    HresvelgrPosition position;
    u8 pad5C[0x40];
    u32 flags;
    u8 padA0[15];
    u8 finished;
    u8 padB0;
    u8 changed;
    u8 padB2[0x23e];
    u8 action[0x10];
    u8 pad300[0x9c];
    HresvelgrPosition savedPosition;
} HresvelgrActor;
void ActionState_Reset(u8 *);
void ActionState_AdvanceState(u8 *);
void Hresvelgr_RefreshTarget(HresvelgrActor *);
void Hresvelgr_PlayDelayedReposition(HresvelgrActor *p)
{
    u8 *action = p->action;
    {
        u32 mask = 0x1c;
        p->flags |= mask;
    }
    if (p->changed) {
        p->changed = 0;
        ActionState_Reset(action);
    }
    switch (*action) {
        case 0:
            if (++*(u32 *)(action + 4) > 119) {
                Hresvelgr_RefreshTarget(p);
                p->position = p->savedPosition;
                p->position.y = 0x2000;
                ActionState_AdvanceState(action);
            }
            break;
        case 1:
            p->finished = 1;
            break;
    }
}
