#define OffsetActor_UpdateFacingWait sub_0819D660
#define OffsetActor_TestFacingSector sub_0819C83C
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
void ActionState_Reset(void *);
void ActionState_SetState(void *, u32);
s32 OffsetActor_TestFacingSector(struct OffsetActor *);
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
void sub_0819D660(struct OffsetActor *p)
{
    struct OffsetActor *obj = p;
    struct OffsetActorAction *s = &p->action;
    p->active = 1;
    if (Enter(p))
        ActionState_Reset(s);
    switch (s->state) {
        case 0:
            if (!s->frame)
                SetAnimation(obj, 1);
            if (OffsetActor_TestFacingSector(obj)) {
                s->state = 1;
                SetAnimation(obj, 2);
            } else
                s->frame++;
            if (s->frame > 0x100)
                ActionState_SetState(s, 3);
            break;
        case 1:
            if (obj->finished)
                ActionState_SetState(s, 0);
            break;
        case 2:
            if (obj->finished)
                ActionState_SetState(s, 0);
            break;
        case 3:
            p->sequenceDone = 1;
            break;
    }
}
