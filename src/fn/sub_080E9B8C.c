#include "global.h"
#include "scene_actor_reaction.h"
void SceneActor_EndReactionAnimation(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        u16 *destination = &actor->colliderA.flags;
        u32 flags = *destination;
        u32 mask = 4;
        mask |= flags;
        mask &= 0xFFF7;
        *destination = mask;
        {
            u32 mask = 0x800;
            mask |= actor->flags2C0;
            actor->flags2C0 = mask;
        }
        SceneActorAnimation_SelectDirectionalFlags((void *)actor, 16);
    }
    if (actor->field2B3) {
        ((struct SceneActorRegistryActor *)actor)->field178 = 0;
        actor->field2B0 = 1;
        if (!actor->drawableKind)
            *(u32 *)actor->drawable |= 1;
        else {
            struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->drawable + 0x20);
            tile->flags |= 1;
        }
    }
}
