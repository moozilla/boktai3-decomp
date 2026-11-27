#include "scene/motion/scene_steering_actor.h"
s32 SparkExplosion_Create(const struct ContactPosition *, u32, u32, s32, s32, s32, s32);
void ScaledShadow_Destroy(void *);
static inline s32 Dimension(u16 *field)
{
    return *field ? *field : 1;
}
void SceneSteeringActor_DestroySlot(struct SceneSteeringActorPool *p,
                                    struct SceneSteeringActor *actor)
{
    if (actor->enter) {
        SparkExplosion_Create(&actor->position, 2, 0, Dimension(&actor->collider.unknown3C),
                            Dimension(&actor->collider.responseParameter), 40, 16);
        actor->stateFrame = 0;
        if (!actor->shadowStatus) {
            ScaledShadow_Destroy(actor->shadow);
            actor->shadowStatus = 255;
        }
        Sprite_Remove(actor);
        p->used &= ~(1 << actor->index);
        actor->index = -1;
        actor->state = 0;
        actor->enter = 0;
    }
}
