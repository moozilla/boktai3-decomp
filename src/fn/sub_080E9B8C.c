#include "global.h"
#include "scene_actor_reaction.h"
void SceneActor_EndReactionAnimation(struct ChaseActor *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        u16 *destination = &actor->base.colliderA.flags;
        u32 flags = *destination;
        u32 mask = 4;
        mask |= flags;
        mask &= 0xFFF7;
        *destination = mask;
        {
            u32 mask = 0x800;
            mask |= actor->base.flags2C0;
            actor->base.flags2C0 = mask;
        }
        SceneActorAnimation_SelectDirectionalFlags((void *)actor, 16);
    }
    if (actor->base.restart) {
        ((struct ChaseActor *)actor)->base.colliderA.responseValue = 0;
        actor->base.field2B0 = 1;
        if (!actor->base.drawableKind)
            *(u32 *)actor->base.drawable |= 1;
        else {
            struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->base.drawable + 0x20);
            tile->flags |= 1;
        }
    }
}
