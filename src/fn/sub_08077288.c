#include "scene_actor_terrain.h"
#include "scene_actor_animation.h"
void SceneActorTerrain_ResumeDrawable(struct RoomActor *actor)
{
    u32 *flags = &actor->behaviorFlags;
    u32 mask = -4;
    *flags &= mask;
    if (!actor->drawableKind) {
        struct SceneActorRegistrySequenceDrawable *drawable = actor->drawable;
        drawable->flags &= mask + 2;
    } else {
        struct SceneInputWidget *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u32 clear = -2;
        tile->flags &= clear;
    }
}
