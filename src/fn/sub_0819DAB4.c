#include "hresvelgr_travel.h"

#include "ram.h"
#include "player.h"

// confidence=low: initial attack sequence before arena departure.

void ActionState_Reset(void *);
void ActionState_SetState(void *, u32);
void ActionState_AdvanceState(void *);

static inline void Request(struct HresvelgrTravelActor *p, u32 kind)
{
    p->requestedAnimation = kind;
}
static inline u32 TestFlags(struct HresvelgrTravelActor *p, u32 mask)
{
    return p->flags & mask;
}
static inline u32 PlayerFlags(struct Player *p, u32 mask)
{
    return p->statusFlags & mask;
}
void Hresvelgr_UpdateOpeningAttackCycle(struct HresvelgrTravelActor *p)
{
    struct HresvelgrTravelActor *q = p;
    struct BossActionState *a = &p->behavior;
    if (Hresvelgr_EnterVulnerableBehavior(p)) {
        u16 *counter = &p->recoveryFrames;
        *counter = 0;
        ActionState_Reset(a);
        ActionState_SetState(a, 0);
    }
    switch (a->state) {
        case 0:
            if (!a->frame) {
                Request(q, 14);
                a->frame++;
            } else if (q->finished)
                ActionState_AdvanceState(a);
            break;
        case 1: {
            u8 *f = &q->behaviorFlags;
            u32 old = *f;
            u32 bits = 2;
            bits |= old;
            *f = bits;
        }
            if (!a->frame) {
                Request(q, 15);
                a->frame++;
            } else if (q->finished)
                ActionState_AdvanceState(a);
            if (TestFlags(q, 0x4000000))
                ActionState_SetState(a, 3);
            break;
        case 2: {
            u8 *f = &q->behaviorFlags;
            u32 old = *f;
            u32 value = 2;
            value |= old;
            value |= 4;
            *f = value;
        }
            if (!a->frame) {
                Request(q, 3);
                a->frame++;
            } else if (q->finished) {
                ActionState_SetState(a, 4);
            }
            if (TestFlags(q, 0x4000000))
                ActionState_SetState(a, 3);
            break;
        case 3:
            if (!a->frame) {
                Request(q, 32);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 4);
            break;
        case 4:
            if (!a->frame) {
                Request(q, 16);
                a->frame++;
            } else {
                u32 mask = 0x40000000;
                u32 flags = q->flags;
                if (flags & mask)
                    ActionState_SetState(a, 7);
                else if ((s32)flags < 0)
                    ActionState_SetState(a, 6);
                else if (q->finished) {
                    if ((flags & 0x20000000) && !PlayerFlags(q->player, 0x40000))
                        ActionState_SetState(a, 5);
                    else
                        ActionState_SetState(a, 9);
                }
            }
            break;
        case 5:
            if (!a->frame) {
                Request(q, 17);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 9);
            break;
        case 6:
            ActionState_SetState(a, 7);
            break;
        case 7:
            if (!a->frame) {
                Request(q, 35);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 8);
            break;
        case 8:
            if (!a->frame) {
                Request(q, 24);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 12);
            break;
        case 9:
            if (!a->frame) {
                Request(q, 18);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 10);
            break;
        case 10:
            if (!a->frame) {
                Request(q, 19);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 11);
            break;
        case 11:
            if (!a->frame) {
                Request(q, 1);
                a->frame++;
            }
            if (q->position.y <= 832)
                ActionState_SetState(a, 12);
            break;
        case 12:
            p->behaviorFinished = 1;
            break;
    }
}
