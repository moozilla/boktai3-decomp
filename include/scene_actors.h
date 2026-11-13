#ifndef GUARD_SCENE_ACTORS_H
#define GUARD_SCENE_ACTORS_H

#include "engine.h"
#include "state_actor.h"
#include "scene_body_api.h"
#include "contact_collider.h"
#include "scene_actor_animation.h"
#include "script_utility.h"

/* Three halfwords passed to position-offset and particle emitters. */
struct SceneActorVector {
    s16 x, y, z;
};

void SceneActors_Register(struct StateActor *actor, u32 slot);
void SceneActors_Unregister(u32 slot);
struct StateActor *SceneActors_GetBySlot(u32 slot);
struct StateActor *SceneActors_FindAvailable(void);
int SceneActors_ApplyDamage(struct ContactCollider *contact, int kind, int amount);
int SceneActors_Initialize(struct StateActor *actor, int id, int slot, void *position,
                           int facingArg);
int SceneActors_Destroy(struct StateActor *actor);
int SceneActors_SetHealthLimits(struct StateActor *actor, int current, int maximum, int extra);
int SceneActors_SetHealth(struct StateActor *actor, int health);
void SceneActors_SetDamageCallback(struct StateActor *actor, int kind,
                                   int (*callback)(void *, int, int), int defense);
int SceneActors_SetShake(struct StateActor *actor, int frames, int strength);
void SceneActors_StartAnimation(struct StateActor *actor, struct SceneActorAnimationTile *animation,
                                int resource, int sequence, int speed, int flags);
void SceneActors_StartDirectionalAnimation(struct StateActor *actor,
                                           struct SceneActorAnimationTile *animation, int resource,
                                           int sequence, int speed, int offset, int flags);
void SceneActors_StartEightDirectionAnimation(struct StateActor *actor,
                                              struct SceneActorAnimationTile *animation,
                                              int resource, int sequence, int speed, int offset,
                                              int flags);
void SceneActors_StartUnmirroredAnimation(struct StateActor *actor,
                                          struct SceneActorAnimationTile *animation, int resource,
                                          int sequence, int speed, int flags);
int SceneActors_UpdateAnimation(struct StateActor *actor, void *animation, int delta);
void SceneActors_UpdateTargetMetrics(struct StateActor *actor);
void SceneActors_UpdateEffects(struct StateActor *actor);
void SceneActors_RunStateCallback(struct StateActor *actor);
void SceneActors_SetEventScripts(struct StateActor *actor, u32 first, u32 second);
void SceneActors_RunFirstEventScript(struct StateActor *actor, s16 *position);
void SceneActors_RunSecondEventScript(struct StateActor *actor, s16 *position);
int SceneActors_CopyAvailablePosition(int *out);
struct StateActorPos *SceneActors_GetAvailablePosition(void);
void SceneActors_ConfigureEffect(struct StateActor *actor, unsigned mode, int first, int second);
void SceneActors_SpawnAmbientParticle(struct StateActor *actor);
void SceneActors_ScriptSetEffects(void);
void SceneBody_SetPositionOffset(void *, void *);
void SceneBody_Unregister(void *);
void SceneMessages_RegisterReceiver(void *, int, int);
void SceneMessages_UnregisterReceiver(void *);
void SceneLink_RemoveNode(void *);
void TargetRegistry_RegisterSceneActor(void *, int, void *);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, int, int, void *, int, int, int);
void Camera_RequestShake(int);
void AnimatedParticles_EmitBurst(void *, int, int, int, int, int, int, int);
int Div(int, int);
extern u32 gUnk_02000560;

/* Keep the older command-pipeline members flat: they share this body storage. */
static inline struct SceneActorBody *SceneActors_Body(struct StateActor *actor)
{
    return (struct SceneActorBody *)&actor->bodyId;
}

static inline void SceneActors_SetFacing(struct StateActor *actor, int angle)
{
    int shifted;
    u32 direction;
    actor->bodyDirection = angle;
    shifted = actor->bodyDirection + 0x60;
    shifted >>= 6;
    direction = 3;
    direction &= shifted;
    actor->animationFacing4 = direction;
    shifted = actor->bodyDirection + 0x70;
    shifted >>= 5;
    direction = 7;
    direction &= shifted;
    actor->animationFacing8 = direction;
}

#endif
