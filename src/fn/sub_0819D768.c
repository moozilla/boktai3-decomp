#include "global.h"

struct ActorActionSequence {
    u8 unknown0[0x56];
    s16 height;
    u8 unknown58[0x9A - 0x58];
    u8 active;
    u8 unknown9B[0xAA - 0x9B];
    u8 animation;
    u8 unknownAB[4];
    u8 animationDone;
    u8 entered;
    u8 unknownB1[0x300 - 0xB1];
    u8 action[0x10];
    u8 unknown310[0x383 - 0x310];
    u8 done;
};
void ActionState_Reset(u8 *);
void ActionState_SetState(u8 *, u32);
void ActionState_AdvanceState(u8 *);
static inline u8 ConsumeEntry(struct ActorActionSequence *p)
{
    if (p->entered) {
        p->entered = 0;
        p->animationDone = 0;
        return 1;
    }
    return 0;
}
void ActorAction_PlayDescentSequence(struct ActorActionSequence *p)
{
    struct ActorActionSequence *actor = p;
    u8 *action = p->action;
    p->active = 1;
    if (ConsumeEntry(p)) {
        ActionState_Reset(action);
        ActionState_SetState(action, 0);
    }
    switch (*action) {
        case 0:
            if (*(u32 *)(action + 4) == 0) {
                u32 animation = 0x15;
                actor->animation = animation;
                (*(u32 *)(action + 4))++;
            } else if (actor->animationDone)
                ActionState_AdvanceState(action);
            break;
        case 1:
            if (*(u32 *)(action + 4) == 0) {
                u32 animation = 0x16;
                actor->animation = animation;
                (*(u32 *)(action + 4))++;
            } else if (actor->animationDone)
                ActionState_AdvanceState(action);
            break;
        case 2:
            if (*(u32 *)(action + 4) == 0) {
                u32 animation = 0x13;
                actor->animation = animation;
                (*(u32 *)(action + 4))++;
            } else if (actor->animationDone)
                ActionState_AdvanceState(action);
            break;
        case 3:
            if (*(u32 *)(action + 4) == 0) {
                u32 animation = 1;
                actor->animation = animation;
            }
            (*(u32 *)(action + 4))++;
            if (actor->height <= 0x340)
                ActionState_SetState(action, 4);
            break;
        case 4:
            p->done = 1;
            break;
    }
}
