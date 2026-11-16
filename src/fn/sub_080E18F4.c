#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
struct SceneActorTimedAttackContext {
    u8 unknown0[0x624];
    struct ContactCollider collider;
};
void FeatherEffectPool_CreateDirectional(struct ContactPosition *, u32, u32, u32);
static inline u8 TakePrepared(struct ChaseActor *a)
{
    if (a->base.field2B2) {
        a->base.field2B2 = 0;
        return 1;
    }
    return 0;
}
void SceneActorAnimation_RefreshTimedAttackCollider(struct ChaseActor *a, u32 frame)
{
    struct SceneActorTimedAttackContext *context = a->base.context;
    if (TakePrepared(a)) {
        u32 variant, palette;
        SceneActorAnimation_SelectDirectionalEntry(&a->base, 17);
        palette = a->base.paletteOffset;
        variant = 0;
        if (palette == 1)
            variant = 1;
        FeatherEffectPool_CreateDirectional(&a->base.body.position, a->base.body.direction, 0,
                                            variant);
        Sound_PlaySong(0x3A5);
    }
    if (frame - 13 <= 10)
        Collider_Refresh(&context->collider);
    if (a->base.restart)
        a->base.field2B0 = 1;
}
