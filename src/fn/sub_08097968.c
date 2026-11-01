#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "player.h"
#include "display_sequence_inline.h"

struct IvyPlacement {
    u32 xy, z;
};

void CpuSet(const void *, void *, u32);
static inline void Fill(void *dst, u32 *tmp, u32 v)
{
    *tmp = v;
    CpuSet(tmp, dst, 0x05000002);
}
struct GhostMovement {
    u8 pad[0x558];
    u32 flags;
    u8 pad2[0x5B4 - 0x55C];
    u16 time;
    u8 phase, active;
    u16 speed;
    u8 pad3[3];
    u8 a, b;
    u8 pad4[0x5EC - 0x5BF];
    u32 fill[6];
    struct IvyPlacement placement;
};

void Ghost_ResetMovementState(struct SceneActorRegistryResetView *actor)
{
    struct GhostMovement *context = actor->context;
    u32 tmp;
    u32 one;
    context->time = 0;
    context->phase = 0;
    {
        u8 *active = &context->active;
        one = 1;
        *active = one;
    }
    context->speed = 0;
    context->a = 0;
    context->b = 0;
    context->flags = actor->saved.privateFlags;
    context->placement = *(struct IvyPlacement *)(((u8 **)gPlayers)[0] + 0x30);
    Fill((u8 *)context + 0x5EC, &tmp, 0);
    Fill((u8 *)context + 0x5F4, &tmp, 0);
    Fill((u8 *)context + 0x5FC, &tmp, 0);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}
