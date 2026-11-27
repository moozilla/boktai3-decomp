#include "global.h"

// confidence=low: dispatches pending scene messages into action and animation state.
struct Vec {
    s16 x, y, z, pad;
};
struct Message {
    u8 pad0[6];
    u8 command;
    u8 pad7;
    s16 args[3];
};
struct Trigger {
    u8 pad0[6];
    u8 count[2];
    struct Message *pending[2][4];
};
struct Hresvelgr {
    u8 pad0[0x51];
    u8 heading;
    u8 pad52[2];
    struct Vec position;
    u8 pad5C[0x34];
    u32 actionFrame;
    u8 pad94[0xe];
    u8 phase;
    u8 padA3;
    u32 phaseFrame;
    u8 direction4, direction8;
    u8 padAA;
    u8 action;
    u8 mode, modePhase, modeParameter;
    u8 padAF;
    u8 modeEntered, entered;
    u8 padB2[0x12];
    u32 modeFrame;
    u8 padC8[0x38];
    void (*callback)(void *);
    u8 pad104[0x14];
    struct Trigger trigger;
    u8 pad140[0x34];
    u8 animationState[0x20];
    u8 animation[0x12];
    u16 sequence;
    u8 pad1A8[4];
    u16 scale;
    u8 pad1AE;
    u8 animationFlags;
    u8 pad1B0[4];
    struct Vec renderPosition;
    u8 pad1BC[0x16f];
    u8 colliderDirection;
};
struct Director {
    u8 pad0[0x24];
    u8 active;
};
extern struct Director *gSceneEventDispatchState;
void SceneMessages_SetPendingTrigger(void *, void *);
void Trigger_ConsumePending(void *, s32);
void Hresvelgr_OnActionTwentyEight(void *);
void Hresvelgr_BeginRestAnimation(void *);
void Hresvelgr_UpdateRandomAnimation(void *);
void Hresvelgr_UpdateRisingProjectile(void *);
void SceneActors_ConfigureEffect(void *, s32, s32, s32);
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);

static inline void SetMode(struct Hresvelgr *p, u32 mode, u32 zero, u32 parameter)
{
    p->mode = mode;
    p->modePhase = zero;
    p->modeParameter = parameter;
    p->modeFrame = zero;
    p->modeEntered = 1;
}
static inline void SetAction(struct Hresvelgr *p, void (*callback)(void *), u8 action)
{
    p->action = action;
    p->callback = callback;
    p->actionFrame = 0;
    p->entered = 1;
    p->phase = 0;
    p->phaseFrame = 0;
}
void Hresvelgr_DispatchSceneMessages(struct Hresvelgr *p)
{
    struct Hresvelgr *actor = p;
    struct Trigger *trigger = &p->trigger;
    s32 i = 0;
    if (i < trigger->count[gSceneEventDispatchState->active]) {
        do {
            struct Director *director = gSceneEventDispatchState;
            struct Message *message = trigger->pending[director->active][i];
            SceneMessages_SetPendingTrigger(trigger, message);
            switch (message->command) {
                case 0:
                    SetMode(p, 2, 0, 0);
                    SetAction(p, Hresvelgr_OnActionTwentyEight, 0x1c);
                    break;

                case 1:
                    SetMode(p, 2, 0, 1);
                    SetAction(p, Hresvelgr_BeginRestAnimation, 0x1d);
                    break;

                case 19:
                    SetMode(p, 2, 0, 2);
                    SetAction(p, Hresvelgr_UpdateRandomAnimation, 0x1e);

                    break;

                case 26:
                    actor->colliderDirection = 1;
                    SetMode(p, 2, 0, 3);
                    SetAction(p, Hresvelgr_UpdateRisingProjectile, 0x1f);
                    break;

                case 9:
                    SceneActors_ConfigureEffect(p, message->args[0], 180, 180);
                    Trigger_ConsumePending(&p->trigger, 1);
                    break;

                case 5:
                    p->heading = (*(u8 *)message->args + 5) << 5;
                    {
                        s32 shifted = (p->heading + 0x60) >> 6;
                        u32 mask = 3;
                        mask &= shifted;
                        p->direction4 = mask;
                    }
                    {
                        s32 shifted = (p->heading + 0x70) >> 5;
                        u32 mask = 7;
                        mask &= shifted;
                        p->direction8 = mask;
                    }
                    actor->colliderDirection = p->direction4 >> 1;
                    Hresvelgr_SetAnimation(actor, actor->animation, actor->animationState,
                                           actor->sequence, actor->animationFlags, actor->scale);
                    Trigger_ConsumePending(&p->trigger, 1);
                    break;

                case 2:
                    p->position.x = message->args[0];
                    p->position.y = message->args[1];
                    p->position.z = message->args[2];
                    actor->renderPosition.x = p->position.x;
                    actor->renderPosition.y = p->position.y;
                    actor->renderPosition.z = p->position.z;
                    Trigger_ConsumePending(&p->trigger, 1);
                    break;
            }
            i++;
        } while (i < trigger->count[gSceneEventDispatchState->active]);
    }
}
