#include "engine/shared/scene_actor_chase.h"
#include "engine/shared/global.h"
#include "actors/scene_actor_placement/scene_actor_placement.h"
#include "engine/shared/scene_actor_chase.h"
s32 Math_Atan2(s32, s32);
void ActorParticles_SpawnBurst(struct RoomActor *);
s32 SceneActorPlacement_BeginDestinationJump(struct RoomActor *actor)
{
    struct ContactPosition delta;
    if (actor->category == 3 && SceneActorPlacement_ReadFlags16(actor, 0x100) &&
        (actor->hitAttributes & 0x400)) {
        Chase_Delta(&delta, &actor->body.position, &actor->reactionStart);
        actor->secondaryMotion.x = Div(delta.x, 36);
        actor->secondaryMotion.y = 20;
        actor->secondaryMotion.z = Div(delta.z, 36);
        actor->body.direction = Math_Atan2(delta.x, delta.z);
        ActorParticles_SpawnBurst(actor);
        SceneActorPlacement_PlayVariantEventSound(actor, 6);
        return 1;
    }
    return 0;
}
