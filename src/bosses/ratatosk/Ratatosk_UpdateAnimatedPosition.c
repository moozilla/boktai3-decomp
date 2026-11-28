#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
s32 Math_Atan2(s32, s32);
s32 Mod(s32, s32);
void Ratatosk_OscillateScale(struct Ratatosk *);
s32 SceneActors_UpdateAnimation(void *, void *, s32);
void Obj_StepPosition(void *);
void ActorProximity_UpdateSquaredDistances(struct Ratatosk *);
void Ratatosk_UpdatePointDistancesToPlayer(struct Ratatosk *);
void SceneDistance_UpdateLinkedDistances(struct Ratatosk *);
void SceneDistance_UpdateLinkedTargetDistances(struct Ratatosk *);
void Ratatosk_DispatchSceneCommands(struct Ratatosk *);
void Ratatosk_UpdateSunOrbPlayerTransfer(struct Ratatosk *);
void Ratatosk_UpdateAnimatedPosition(struct Ratatosk *p)
{
    struct Ratatosk *actor = p;
    struct SceneActorBody *body = &p->body;
    p->actionAnimation = 0;
    Ratatosk_DispatchSceneCommands(p);
    Obj_StepPosition(body);
    if (Mod(p->callbackFrame, 30) == 0) {
        ActorProximity_UpdateSquaredDistances(p);
        SceneDistance_UpdateLinkedDistances(p);
    }
    if (Mod(p->callbackFrame, 30) == 15) {
        Ratatosk_UpdatePointDistancesToPlayer(p);
        SceneDistance_UpdateLinkedTargetDistances(p);
    }
    Ratatosk_UpdateSunOrbPlayerTransfer(actor);
    SceneActors_UpdateAnimation(actor, actor->animationData, (s32)actor->animation);
    Ratatosk_OscillateScale(actor);
    actor->anchor.x = body->position.x;
    actor->anchor.y = body->position.y + (actor->distance >> 4);
    actor->anchor.z = body->position.z;
    if (actor->playerPosition) {
        s32 dx = actor->playerPosition->x - body->position.x;
        s32 dz = actor->playerPosition->z - body->position.z;
        actor->requestedAngle = Math_Atan2(dx, dz);
        actor->playerDistance = dx * dx + dz * dz;
    }
}
