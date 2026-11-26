#include "scene/motion/scene_actor_terrain.h"
#include "scene/actor/scene_actor_state.h"
#include "scene/actor/scene_actor_registry.h"
#include "player/player.h"
#include "scene/actor/scene_actor_status.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline u8 IsPlayerRelativeCategory(u32 c)
{
    if (c - 0x11 <= 3 || c == 0x17)
        return 1;
    return 0;
}
void SceneActorMovement_UpdateHeight(struct ChaseActor *actor)
{
    u32 category = actor->base.category;
    if (category == 0x16) {
        u32 height = SceneActorTerrain_GetHeight(actor->base.terrainCell, &actor->base.body.position);
        u16 *ground = &actor->base.groundHeight;
        if (*ground != height) {
            u16 *displacement = &actor->base.heightOffset;
            *displacement -= height - *ground;
        }
        *ground = height;
    } else {
        u32 mask = 0x4000;
        if (!(actor->base.motionFlags & mask) && IsPlayerRelativeCategory(category)) {
            u32 height = (u16)gPlayers[0]->position.y;
            actor->base.groundHeight = height;
        } else {
            u32 height = SceneActorTerrain_GetHeight(actor->base.terrainCell, &actor->base.body.position);
            actor->base.groundHeight = height;
        }
    }
    if (!ActorStatus_BodyFlags(actor, 0x20)) {
        u16 height = actor->base.groundHeight;
        actor->base.body.position.y = height;
        actor->base.body.position.y = height + actor->base.heightOffset;
    }
    if (actor->base.drawableKind == 0)
        ((struct SceneActorRegistrySequenceDrawable *)actor->base.drawable)->position =
            actor->base.body.position;
    else
        ((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->position =
            actor->base.body.position;
}
