// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "display_sequence_inline.h"

struct IvyContext {
    u8 unknown0[0x62C];
    u32 flags;
    u8 unknown630[4];
    u16 field634;
};

struct IvyPlacement {
    u32 xy, z;
};

void CpuSet(const void *, void *, u32);
static inline void Fill(void *dst, u32 *tmp, u32 v)
{
    *tmp = v;
    CpuSet(tmp, dst, 0x05000002);
}
struct LunMovement {
    u8 pad[0x3d4];
    u32 flags;
    u8 pad2[8];
    u16 time;
    u8 phase, active;
    u16 speed;
    u8 a, b;
};
// confidence=low: resets movement interpolation and starts the directional animation.
void Lun_ResetMovementState(struct SceneActorRegistryResetView *actor)
{
    struct LunMovement *context = actor->context;
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
    context->flags = 0;
    Fill((u8 *)context + 0x414, &tmp, 0);
    Fill((u8 *)context + 0x41c, &tmp, 0);
    Fill((u8 *)context + 0x424, &tmp, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    {
        register void *drawable asm("r0") = actor->drawable;
        asm("" : "+r"(drawable));
        DisplaySequence_Step(drawable);
    }
}
