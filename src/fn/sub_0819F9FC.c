#include "global.h"
struct ActorReaction {
    u8 unknown0[0x51];
    u8 direction;
    u8 unknown52[0x4A];
    u32 flags;
    u8 unknownA0[3];
    u8 animationCycles;
    u8 unknownA4[4];
    u8 facing4, facing8;
    u8 unknownAA[5];
    u8 finished, unknownB0, pending;
    u8 unknownB2[0x1A];
    u8 targetDirection;
    u8 unknownCD[0x37];
    void *callback;
    u8 unknown108[0x6C];
    u8 collider174[0x20], collider194[0x20];
    u8 unknown1B4[0x13C];
    u8 action[0x10];
};
void ActionState_Reset(u8 *);
void ActionState_SetState(u8 *, u32);
void ActionState_AdvanceState(u8 *);
void Hresvelgr_SetAnimation(struct ActorReaction *, void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
void Hresvelgr_EmitFacingMotion(struct ActorReaction *);
s32 Callback_InvokeActor(void *, void *);
void Hresvelgr_UpdateFacingActionPhase(struct ActorReaction *actor)
{
    u8 *action = actor->action;
    if (actor->pending) {
        actor->pending = 0;
        ActionState_Reset(action);
        ActionState_SetState(action, 0);
        actor->direction = actor->targetDirection;
        {
            u32 t;
            s32 sh = (actor->direction + 96) >> 6;
            t = 3;
            t &= sh;
            actor->facing4 = t;
            sh = (actor->direction + 112) >> 5;
            t = 7;
            t &= sh;
            actor->facing8 = t;
        }
    }
    {
        u32 mask = 0x600040;
        actor->flags = (actor->flags | mask) | 0x2000000;
    }
    switch (*action) {
        case 0:
            if (*(u32 *)(action + 4) == 0) {
                Hresvelgr_SetAnimation(actor, actor->collider194, actor->collider174, 19, 1, 0);
                (*(u32 *)(action + 4))++;
                Sound_PlaySong(0x3E2);
                Sound_PlaySong(0x83);
            }
            if (actor->animationCycles > 4)
                ActionState_AdvanceState(action);
            Hresvelgr_EmitFacingMotion(actor);
            break;
        case 1:
            actor->finished = 1;
            break;
    }
    Callback_InvokeActor(actor, actor->callback);
}
