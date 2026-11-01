#include "scene_steering_actor.h"
#include "player_cue_sequence.h"
s32 Mod(s32, s32);
s32 Div(s32, s32);
void SceneSteeringActor_EmitRandomParticle(struct ContactPosition *);
void SceneSteeringActor_Appear(struct SceneSteeringActorPool *unused, struct SceneSteeringActor *p)
{
    if (p->enter) {
        p->flags = (p->flags & ~1) | 2;
        p->scaleTimer = 15;
        p->stateFrame = 0;
        p->enter = 0;
        p->scaleX = 1;
        p->scaleY = 1;
    }
    if (Mod(p->stateFrame, 8) == 0)
        SceneSteeringActor_EmitRandomParticle(&p->position);
    if (p->scaleTimer > 0) {
        p->scaleX += Div(64 - p->scaleX, p->scaleTimer);
        p->scaleY += Div(64 - p->scaleY, p->scaleTimer);
        p->scaleTimer--;
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)p, &p->sequence);
    if (p->scaleTimer <= 0) {
        p->state = 4;
        p->flags &= ~2;
        p->enter = 1;
    } else {
        p->stateFrame++;
    }
}
