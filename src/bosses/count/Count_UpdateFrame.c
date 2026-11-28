#include "global.h"
#include "engine/engine.h"
#include "npc/story_character/state_actor.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
#include "scene/actor/scene_actors.h"
#include "scene/motion/scene_actor_charge_offset.h"
#include "bosses/count/scene_graphics_actor.h"
// confidence=low: Count per-frame update: animation, scale wobble, shake, cached target bearings.
void Obj_StepPosition(void *);
void SceneActors_DispatchCommands(void *);
void SceneActorGraphics_OscillateScale(void *);
s32 Math_Atan2(s32, s32);

void Count_UpdateFrame(struct StateActor *p)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    struct SceneGraphicsActor *owner = (struct SceneGraphicsActor *)p;
    struct SceneActorBody *body = &actor->body;
    s32 i;
    SceneActors_DispatchCommands(actor);
    Obj_StepPosition(body);
    SceneActors_UpdateAnimation((struct StateActor *)owner, &actor->animation,
                                (int)&actor->animationResource);
    SceneActorGraphics_OscillateScale(owner);
    body->position.y = actor->graphics.recoveryGroundHeight + (actor->graphics.scale >> 4);
    actor->animation.position.y = body->position.y;
    actor->animation.position.x = body->position.x;
    actor->animation.position.z = body->position.z;
    if (SceneActorState_TestFlags((struct StateActor *)owner, 0x20000)) {
        actor->animation.position.x +=
            SceneActor_ChargeComponent(gUnk_086149C4[(actor->graphics.shakeAngle + 64) & 255] *
                                       (actor->graphics.shakeStrength << 2));
        actor->animation.position.z += SceneActor_ChargeComponent(
            gUnk_086149C4[actor->graphics.shakeAngle] * (actor->graphics.shakeStrength << 2));
    }
    if (actor->graphics.reactionDelay) {
        u32 angle;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        angle = *(u16 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
        actor->animation.position.x += SceneActor_ChargeComponent(
            gUnk_086149C4[(angle + 64) & 255] * (actor->graphics.reactionDelay << 2));
        actor->animation.position.z += SceneActor_ChargeComponent(
            gUnk_086149C4[angle & 255] * (actor->graphics.reactionDelay << 2));
        actor->graphics.reactionDelay--;
    }
    SceneActors_UpdateTargetMetrics((struct StateActor *)owner);
    for (i = 0; i < actor->playerCount; i++) {
        struct SceneActorBody *base = &owner->body;
        struct ContactPosition *target = owner->playerPositions[i];
        if (target) {
            s32 x = target->x - base->position.x;
            s32 z = target->z - base->position.z;
            owner->playerBearings[i] = Math_Atan2(x, z);
            owner->playerDistances[i] = x * x + z * z;
        }
    }
}
