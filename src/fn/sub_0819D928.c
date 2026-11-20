#include "hresvelgr_travel.h"

#include "ram.h"

// confidence=low: repeats charges until terrain or the attempt limit ends them.

void ActionState_Reset(void *);
void ActionState_SetState(void *, u32);
void ActionState_AdvanceState(void *);

static inline void Request(struct HresvelgrTravelActor *p, u32 kind)
{
    p->requestedAnimation = kind;
}
s32 Hresvelgr_HasCellOverride(void *);
static inline s32 FacesBoundary(struct HresvelgrTravelActor *p)
{
    s32 direction = p->targetHeading;
    s32 result;
    if (!p->side) {
        direction = (direction + 32) & 255;
        if (direction > 64)
            goto outside;
        return 1;
    } else {
        direction = (direction - 32) & 255;
        result = 1;
        if (direction > 64) {
        outside:
            result = 0;
        }
    }
    return result;
}
void Hresvelgr_UpdateRepeatedCharge(struct HresvelgrTravelActor *p)
{
    struct HresvelgrTravelActor *q = p;
    struct BossActionState *a = &p->behavior;
    if (Hresvelgr_EnterVulnerableBehavior(p)) {
        p->chargeCount = 0;
        ActionState_Reset(a);
        ActionState_SetState(a, 0);
    }
    switch (a->state) {
        case 0:
            if (!a->frame) {
                Request(q, 24);
                a->frame++;
            } else if (q->finished)
                ActionState_AdvanceState(a);
            break;
        case 1:
            if (!a->frame) {
                if (Hresvelgr_HasCellOverride(q)) {
                    ActionState_SetState(a, 3);
                    break;
                }
                Request(q, 26);
                (p->chargeCount)++;
                a->frame++;
            } else if (q->finished) {
                if (p->chargeCount > 3)
                    ActionState_SetState(a, 4);
                else if (FacesBoundary(q))
                    ActionState_SetState(a, 1);
                else
                    ActionState_SetState(a, 2);
            }
            break;
        case 2:
            if (!a->frame) {
                Request(q, 2);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 1);
            break;
        case 3:
            if (!a->frame) {
                Request(q, 25);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 1);
            break;
        case 4:
            if (!a->frame) {
                Request(q, 12);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 5);
            break;
        case 5:
            p->behaviorFinished = 1;
            break;
    }
}
