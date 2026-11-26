#include "global.h"
struct OffsetActorAction {
    u8 state, pad1[3];
    u32 frame, unk8, unkC;
};
struct OffsetActor {
    u8 pad0[0x9A];
    u8 active;
    u8 pad9B[0xAA - 0x9B];
    u8 animation;
    u8 padAB[4];
    u8 finished, entered;
    u8 padB1[0x300 - 0xB1];
    struct OffsetActorAction action;
    u8 pad310[0x383 - 0x310];
    u8 sequenceDone;
};
void ActionState_AdvanceState(void *);
void ActionState_Reset(void *);
void ActionState_SetState(void *, u32);
s32 Hresvelgr_HasAdjacentOverride(struct OffsetActor *);
static inline u8 Enter(struct OffsetActor *p)
{
    if (p->entered) {
        p->entered = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}
static inline void SetAnimation(struct OffsetActor *p, u32 id)
{
    p->animation = id;
}
void OffsetActor_UpdateExtendedAnimationSequence(struct OffsetActor *p)
{
    struct OffsetActor *obj = p;
    struct OffsetActorAction *s = &p->action;
    p->active = 1;
    if (Enter(p))
        ActionState_Reset(s);
    switch (s->state) {
        case 0:
            if (!s->frame) {
                SetAnimation(obj, 24);
                s->frame++;
            } else if (obj->finished) {
                if (Hresvelgr_HasAdjacentOverride(obj))
                    ActionState_SetState(s, 1);
                else
                    ActionState_SetState(s, 2);
            }
            break;
        case 1:
            if (!s->frame) {
                SetAnimation(obj, 25);
                s->frame++;
            } else if (obj->finished)
                ActionState_SetState(s, 2);
            break;
        case 2:
            if (!s->frame) {
                SetAnimation(obj, 3);
                s->frame++;
            } else if (obj->finished)
                ActionState_AdvanceState(s);
            break;
        case 3:
            if (!s->frame) {
                SetAnimation(obj, 4);
                s->frame++;
            } else if (obj->finished)
                ActionState_SetState(s, 4);
            break;
        case 4:
            if (!s->frame) {
                SetAnimation(obj, 6);
                s->frame++;
            } else if (obj->finished)
                ActionState_SetState(s, 5);
            break;
        case 5:
            if (!s->frame) {
                SetAnimation(obj, 12);
                s->frame++;
            } else if (obj->finished)
                ActionState_SetState(s, 6);
            break;
        case 6:
            p->sequenceDone = 1;
            break;
    }
}
