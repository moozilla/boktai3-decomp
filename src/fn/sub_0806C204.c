#include "scene_steering_actor.h"
#include "player_cue_sequence.h"
s32 Mod(s32, s32);
void SceneSteeringActor_EmitRandomParticle(struct ContactPosition *);
void SceneSteeringActor_RefreshCollider(struct SceneSteeringActor *);
void ScaledShadow_Enable(void *);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void SceneSteeringActor_BobInPlace(struct SceneSteeringActorPool *unused,
                                   struct SceneSteeringActor *p)
{
    if (p->enter) {
        p->stateFrame = 0;
        ScaledShadow_Enable(p->shadow);
        p->enter = 0;
    }
    {
        struct ContactPosition *velocity = &p->velocity;
        struct ContactPosition *position;
        velocity->x = 0;
        p->velocity.z = 0;
        p->velocity.y = Truncate(gUnk_086149C4[*(u8 *)&p->stateFrame] * 4);
        position = &p->position;
        p->position.x += velocity->x;
        position->y += velocity->y;
        position->z += velocity->z;
        if (Mod(p->stateFrame, 128) == 0)
            SceneSteeringActor_EmitRandomParticle(position);
    }
    SceneSteeringActor_RefreshCollider(p);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)p, &p->sequence);
    p->stateFrame = (p->stateFrame + 2) & 255;
}
