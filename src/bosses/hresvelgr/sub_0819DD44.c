#include "bosses/hresvelgr/hresvelgr_travel.h"

#include "engine/shared/ram.h"


void Hresvelgr_UpdateAttackCycle(struct HresvelgrTravelActor *p)
{
    struct HresvelgrTravelActor *q = p;
    struct BossActionState *a = &p->behavior;
    if (Hresvelgr_EnterVulnerableBehavior(p)) {
        u16 *counter = &p->recoveryFrames;
        u32 zero = 0;
        *counter = zero;
        gHresvelgrAttackCount = zero;
        ActionState_Reset(a);
        ActionState_SetState(a, 0);
    }
    switch (a->state) {
        case 0:
            if (!a->frame) {
                Hresvelgr_SetActionAnimation(q, 14);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 1);
            break;
        case 1: {
            u8 *f = &q->behaviorFlags;
            u32 old = *f;
            u32 bits = 2;
            bits |= old;
            *f = bits;
        }
            if (!a->frame) {
                Hresvelgr_SetActionAnimation(q, 15);
                a->frame++;
            } else if (q->finished)
                ActionState_AdvanceState(a);
            if (Hresvelgr_TestFlags(q, 0x4000000))
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
                Hresvelgr_SetActionAnimation(q, 3);
                a->frame++;
                break;
            } else if (q->finished) {
                ActionState_SetState(a, 4);
                break;
            }
            if (Hresvelgr_TestFlags(q, 0x4000000))
                ActionState_SetState(a, 3);
            break;
        case 3:
            if (!a->frame) {
                Hresvelgr_SetActionAnimation(q, 32);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 4);
            break;
        case 4:
            if (!a->frame) {
                Hresvelgr_SetActionAnimation(q, 16);
                a->frame++;
                gHresvelgrAttackCount++;
            } else {
                if (Hresvelgr_TestFlags(q, 0x40000000))
                    ActionState_SetState(a, 7);
                else if ((s32)q->flags < 0)
                    ActionState_SetState(a, 6);
                else if (q->finished) {
                    if (Hresvelgr_TestFlags(q, 0x20000000))
                        ActionState_SetState(a, 5);
                    else
                        ActionState_SetState(a, 10);
                }
            }
            break;
        case 5:
            if (!a->frame) {
                Hresvelgr_SetActionAnimation(q, 17);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 9);
            break;
        case 6:
            ActionState_SetState(a, 7);
            break;
        case 7:
            if (!a->frame) {
                Hresvelgr_SetActionAnimation(q, 35);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 8);
            break;
        case 8:
            if (!a->frame) {
                Hresvelgr_SetActionAnimation(q, 24);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 14);
            break;
        case 9:
            if (!a->frame) {
                Hresvelgr_SetActionAnimation(q, 18);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 12);
            break;
        case 10:
            if (!a->frame) {
                Hresvelgr_SetActionAnimation(q, 18);
                a->frame++;
            } else if (q->finished) {
                if (gHresvelgrAttackCount <= 5)
                    ActionState_SetState(a, 11);
                else
                    ActionState_SetState(a, 12);
            }
            break;
        case 11:
            if (!a->frame) {
                Hresvelgr_SetActionAnimation(q, 20);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 4);
            break;
        case 12:
            if (!a->frame) {
                Hresvelgr_SetActionAnimation(q, 19);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 13);
            break;
        case 13:
            if (!a->frame) {
                Hresvelgr_SetActionAnimation(q, 1);
                a->frame++;
            }
            if (q->position.y <= 832)
                ActionState_SetState(a, 14);
            break;
        case 14:
            p->behaviorFinished = 1;
            break;
    }
}
