#include "hresvelgr_travel.h"

#include "ram.h"

// confidence=low: requests movement around adjacent terrain overrides.

void ActionState_Reset(void *);
void ActionState_SetState(void *, u32);
void ActionState_AdvanceState(void *);

static inline void Request(struct HresvelgrTravelActor *p, u32 kind)
{
    p->requestedAnimation = kind;
}
s32 Hresvelgr_HasAdjacentOverride(void *);
void Hresvelgr_UpdateTerrainManeuver(struct HresvelgrTravelActor *p)
{
    struct HresvelgrTravelActor *q = p;
    struct BossActionState *a = &p->behavior;
    if (Hresvelgr_EnterVulnerableBehavior(p))
        ActionState_Reset(a);
    switch (a->state) {
        case 0:
            if (!a->frame) {
                Request(q, 24);
                a->frame++;
            } else if (q->finished) {
                if (Hresvelgr_HasAdjacentOverride(q))
                    ActionState_SetState(a, 1);
                else
                    ActionState_SetState(a, 2);
            }
            break;
        case 1:
            if (!a->frame) {
                Request(q, 25);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 2);
            break;
        case 2:
            if (!a->frame) {
                Request(q, 3);
                a->frame++;
            } else if (q->finished)
                ActionState_AdvanceState(a);
            break;
        case 3:
            if (!a->frame) {
                Request(q, 4);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 5);
            break;
        case 4:
            if (!a->frame) {
                Request(q, 6);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 6);
            break;
        case 5:
            if (!a->frame) {
                Request(q, 5);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 4);
            break;
        case 6:
            if (!a->frame) {
                Request(q, 7);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 7);
            break;
        case 7:
            if (!a->frame) {
                Request(q, 12);
                a->frame++;
            } else if (q->finished)
                ActionState_SetState(a, 8);
            break;
        case 8:
            p->behaviorFinished = 1;
            break;
    }
}
