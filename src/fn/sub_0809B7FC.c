#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"

// confidence=low: resumes pursuit or waiting after the lifted-player penalty window.

void SceneActorControl_ConsumeLiftFlagPenalty(void *);
void SceneActorControl_ResetCollisionFlags(void *);
void Lun_BeginMoveToPosition(void *, void *);
void Lun_UpdateWaitingAnimation(void);

static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *a, void *callback, u32 kind,
                              u32 zero, u8 zero8)
{
    a->field2B2 = 1;
    a->field2B0 = zero8;
    a->field2B5 = kind;
    a->target = callback;
    a->field2C8 = zero;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = zero;
}
static inline void SetState(struct SceneActorRegistryResetView *a, u32 phase, u32 zero, u8 zero8)
{
    u32 one = 1;
    a->state2A8 = zero8;
    a->phase2A9 = phase;
    a->behavior2AA = one;
    a->counter2AC = zero;
    a->field2B1 = one;
}
void Lun_ResumeAfterLiftPenalty(struct SceneActorRegistryResetView *input, s32 elapsed)
{
    s32 frame = elapsed;
    u8 zero8;
    if (TakeFlag(&input->field2B2))
        SceneActorControl_ConsumeLiftFlagPenalty(input);
    {
        u32 flags = 0x10;
        u16 *p = &input->flags2D6;
        u32 value = *p;
        u16 initialState[1] = {0};
        zero8 = initialState[0];
        flags |= value;
        *p = flags;
    }
    {
        u32 mask = 0x100000;
        u32 *p = &input->flags2D0;
        *p |= mask;
    }
    if (frame > 30) {
        u8 *ctx = input->context;
        u16 delay;
        SceneActorControl_ResetCollisionFlags(input);
        delay = *((u16 *)(ctx + 0x444));
        if (delay) {
            u32 flags = 0x100;
            u16 *p = &input->flags2D4;
            flags = flags | (*p);
            *p = flags;
            *((u16 *)(ctx + 0x444)) = *((u16 *)(ctx + 0x442));
            Lun_BeginMoveToPosition(input, (*(u8 **)gPlayers) + 0x30);
            SetState(input, 2, 0, zero8);
        } else {
            u32 zero = delay;
            SetControl(input, Lun_UpdateWaitingAnimation, 10, zero, zero8);
            SetState(input, 1, zero, zero8);
            input->body.direction = input->tracking.heading;
            input->field2C2 = 64;
        }
    }
}
