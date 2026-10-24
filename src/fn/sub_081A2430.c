#include "global.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: dispatches pending scene messages into action and animation state.

// confidence=low: dispatches scene messages into action and animation state.
// MATCHING_CONCESSION: register-pinning

// Remaining differences: trigger indexing and shared mode/action constant lifetimes.
// confidence=low: dispatches pending cutscene messages into action and animation state.
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
extern struct Director *gUnk_0200047C;
void SceneMessages_SetPendingTrigger(void *, void *);
void Trigger_ConsumePending(void *, s32);
void Hresvelgr_OnActionTwentyEight(void *);
void Hresvelgr_BeginRestAnimation(void *);
void Hresvelgr_UpdateRandomAnimation(void *);
void Hresvelgr_UpdateRisingProjectile(void *);
void SceneActors_ConfigureEffect(void *, s32, s32, s32);
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);

static inline void SetMode(struct Hresvelgr *p, u32 mode, u32 zero, u32 parameter, u32 one)
{
    p->mode = mode;
    p->modePhase = zero;
    p->modeParameter = parameter;
    p->modeFrame = zero;
    p->modeEntered = one;
}
static inline void SetAction(struct Hresvelgr *p, u8 action, void (*callback)(void *))
{
    p->action = action;
    p->callback = callback;
    p->actionFrame = 0;
    p->entered = 1;
    p->phase = 0;
    p->phaseFrame = 0;
}
static inline void SetMode_1(struct Hresvelgr *p, u32 parameter, u32 one)
{
    {
        u8 k1 = 2;
        p->mode = k1;
    }
    p->modePhase = 0;
    p->modeParameter = parameter;
    p->modeFrame = 0;
    p->modeEntered = one;
}

static inline void SetAction_1(struct Hresvelgr *p, void (*callback)(void *))
{
    {
        u8 k1 = 0x1f;
        p->action = k1;
    }
    p->callback = callback;
    p->actionFrame = 0;
    p->entered = 1;
    p->phase = 0;
    p->phaseFrame = 0;
}

inline static void SetAction_(struct Hresvelgr *p, u8 action, void (*callback)(void *))
{
    p->action = action;
    p->callback = callback;
    {
        typeof(p->actionFrame) *q1 = &p->actionFrame;
        u8 z1 = 1;
        *q1 = 0;
        p->entered = z1;
        p->phase = 0;
        p->phaseFrame = 0;
    }
}

void Hresvelgr_DispatchSceneMessages(struct Hresvelgr *p)
{
    struct Hresvelgr *actor = p;
    struct Trigger *trigger = &p->trigger;
    s32 i = 0;
    for (; i < trigger->count[gUnk_0200047C->active]; i++) {
        register void (*callback)(void *) asm("r1");
        register u32 action asm("r0");
        s32 off1 = i << 2;
        struct Message *message =
            *((struct Message **)((((gUnk_0200047C->active << 4) + off1) + ((u8 *)trigger)) + 8));
        SceneMessages_SetPendingTrigger(trigger, message);
        switch (message->command) {
            case 0:
                SetMode_1(p, 0, 1);
                callback = Hresvelgr_OnActionTwentyEight;
                action = 0x1c;
                goto setAction;

            case 1:
                SetMode(p, 2, 0, 1, 1);
                callback = Hresvelgr_BeginRestAnimation;
                action = 0x1d;
                goto setAction;

            case 19:
                SetMode(p, 2, 0, 2, 1);
                callback = Hresvelgr_UpdateRandomAnimation;
                action = 0x1e;
            setAction:
                SetAction_(p, action, callback);

                break;

            case 26:
                actor->colliderDirection = 1;
                SetMode(p, 2, 0, 3, 1);
                SetAction_1(p, Hresvelgr_UpdateRisingProjectile);
                break;

            case 9:
                SceneActors_ConfigureEffect(p, message->args[0], 180, 180);
                Trigger_ConsumePending(&p->trigger, 1);
                break;

            case 5:
                p->heading = ((*((u8 *)message->args)) + 5) << 5;
                p->direction4 = 3 & ((p->heading + 0x60) >> 6);
                p->direction8 = 7 & ((p->heading + 0x70) >> 5);
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
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Hresvelgr_DispatchSceneMessages);
#endif
