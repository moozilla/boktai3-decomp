#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
void SceneActor_UpdateForwardCollider(struct SceneActorRegistryResetView *);
struct TargetedAttackContext {
    u8 unknown0[0x628];
    struct ContactCollider collider;
    u8 unknown67C[0x6D8 - 0x67C];
    struct ContactPosition target;
};
struct TargetedAttackSequence {
    u8 unknown0[0x14];
    u16 frame;
};
struct TargetedAttackTarget {
    u8 unknown0[0xC];
    struct ContactPosition position;
};
static inline u8 TakeEntryFlag(struct SceneActorRegistryResetView *s)
{
    if (s->field2B2) {
        s->field2B2 = 0;
        return 1;
    }
    return 0;
}
void SceneActor_UpdateTargetedAttack(struct SceneActorRegistryResetView *s)
{
    struct TargetedAttackContext *context = s->context;
    struct TargetedAttackSequence *sequence = (void *)((u8 *)s->drawable + 0x20);
    if (TakeEntryFlag(s)) {
        s->body.direction = (((s->tracking.heading + 0x20) & 0xFF) >> 6) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)s, 5);
        context->target = ((struct TargetedAttackTarget *)s->tracking.target)->position;
        Sound_PlaySong(0x1D0);
    }
    if ((u16)(sequence->frame - 3) <= 1) {
        SceneActor_UpdateForwardCollider(s);
        Collider_Refresh(&context->collider);
    }
    if (s->field2B3)
        s->field2B0 = 1;
}
