#include "global.h"
struct ActorReaction {
    u8 unknown0[0x9C];
    u32 flags;
    u8 unknownA0[2];
    u8 recoveryState, animationCycles;
    u32 timer;
    u8 unknownA8[7];
    u8 finished;
    u8 unknownB0;
    u8 pending;
    u8 unknownB2[0x52];
    void *callback;
    u8 unknown108[0x6C];
    u8 collider174[0x20];
    u8 collider194[0x20];
};
void Hresvelgr_SetAnimation(struct ActorReaction *, void *, void *, u32, u32, u32);
void Anim_SetFrameDurationScale(void *, u32);
s32 Callback_InvokeActor(void *, void *);
void Hresvelgr_UpdateRecoveryPhase(struct ActorReaction *actor)
{
    if (actor->pending) {
        actor->pending = 0;
        Hresvelgr_SetAnimation(actor, actor->collider194, actor->collider174, 16, 1, 0);
        Anim_SetFrameDurationScale(actor->collider194, 160);
        actor->recoveryState = 0;
        actor->timer = 0;
    }
    {
        u32 mask = 0x200040;
        actor->flags |= mask;
    }
    if ((u8)Callback_InvokeActor(actor, actor->callback) == 0) {
        switch (actor->recoveryState) {
            case 0:
                if (actor->animationCycles > 3) {
                    actor->recoveryState = 1;
                    actor->timer = 0;
                    Anim_SetFrameDurationScale(actor->collider194, 64);
                }
                break;
            case 1:
                actor->finished = 1;
                break;
        }
    }
}
