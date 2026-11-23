#include "global.h"
#include "hresvelgr_travel.h"

// confidence=low: prioritizes defeat, recovery, and interrupted charge reactions.
void sub_081A14FC(void *);
void sub_081A0FF0(void *);
void Hresvelgr_UpdateTimedPose(void *);
void Hresvelgr_UpdateMotionPhase(void *);

s32 Hresvelgr_TryStartReaction(struct HresvelgrTravelActor *p)
{
    struct HresvelgrTravelActor *q = p;
    if (p->health <= 0 && p->vulnerable) {
        Hresvelgr_SetFlags(p, 2);
        Hresvelgr_SetActionAnimation(p, 38);
    }
    p->vulnerable = 0;
    if (Hresvelgr_TestFlags(p, 2)) {
        Hresvelgr_StartAction(p, 1, 0, 5);
        Hresvelgr_StartReaction(p, sub_081A14FC, 38);
        return 1;
    }
    if (p->flags & 1) {
        q->savedPhase = p->actionPhase;
        Hresvelgr_StartAction(p, 1, 0, 0);
        Hresvelgr_StartReaction(p, sub_081A0FF0, 34);
        return 1;
    }
    if (p->flags & 0x10000000) {
        q->savedPhase = p->actionPhase;
        Hresvelgr_StartAction(p, 1, 0, 1);
        Hresvelgr_StartReaction(p, Hresvelgr_UpdateTimedPose, 35);
        return 1;
    }
    if (p->flags & 0x100000) {
        q->savedPhase = p->actionPhase;
        q->savedCallback = p->actionCallback;
        q->savedAnimation = p->currentAnimation;
        Hresvelgr_StartAction(p, 1, 0, 2);
        Hresvelgr_StartReaction(p, Hresvelgr_UpdateMotionPhase, 32);
        return 1;
    }
    return 0;
}
