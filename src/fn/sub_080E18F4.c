#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
struct SceneActorTimedAttackContext {
    u8 unknown0[0x624];
    struct ContactCollider collider;
};
void FeatherEffectPool_CreateDirectional(struct ContactPosition *, u32, u32, u32);
static inline u8 TakePrepared(struct SceneActorRegistryResetView *a)
{
    if (a->field2B2) {
        a->field2B2 = 0;
        return 1;
    }
    return 0;
}
void SceneActorAnimation_RefreshTimedAttackCollider(struct SceneActorRegistryResetView *a,
                                                    u32 frame)
{
    struct SceneActorTimedAttackContext *context = a->context;
    if (TakePrepared(a)) {
        u32 variant, palette;
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)a, 17);
        palette = a->paletteOffset;
        variant = 0;
        if (palette == 1)
            variant = 1;
        FeatherEffectPool_CreateDirectional(&a->body.position, a->body.direction, 0, variant);
        Sound_PlaySong(0x3A5);
    }
    if (frame - 13 <= 10)
        Collider_Refresh(&context->collider);
    if (a->field2B3)
        a->field2B0 = 1;
}
