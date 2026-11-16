#include "global.h"
#include "scene_actor_placement.h"
s32 Math_Atan2(s32, s32);
void ActorParticles_SpawnBurst(struct RoomActor *);
static inline void Difference(struct ContactPosition *out, const struct ContactPosition *b,
                              const struct ContactPosition *a)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
s32 SceneActorPlacement_BeginDestinationJump(struct RoomActor *actor)
{
    struct ContactPosition delta;
    if (actor->category == 3 && SceneActorPlacement_ReadFlags16(actor, 0x100) &&
        (actor->hitAttributes & 0x400)) {
        Difference(&delta, &actor->body.position, &actor->reactionStart);
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
