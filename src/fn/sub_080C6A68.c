#include "global.h"
#include "golem_charge.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"

void sub_080C47B8(struct ContactPosition *);
s32 sub_080C47DC(void *);

/* Decelerate, approach the current cell center, then restore route motion. */
void Golem_UpdateChargeRecovery(struct SceneActorRegistryResetView *actor)
{
    struct GolemChargeContext *context = actor->context;
    struct SceneActorBody *body = &actor->body;
    u8 directionVariant, mirror;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->field2B0 = 0;
        actor->field2B3 = 0;
        context->charge.phase = 0;
        {
            u32 angle = (context->charge.savedDirection + 0x60) >> 6;
            u32 quadrant = 3;
            quadrant &= angle;
            context->charge.quadrant = quadrant;
        }
        context->charge.stopPosition = body->position;
        sub_080C47B8(&context->charge.stopPosition);
    }
    Golem_SetMotionFlags(actor, 0x100);
    Golem_GetDirectionVariant(actor, &directionVariant, &mirror);
    Golem_SelectAnimation(actor, 8, directionVariant, Golem_MirrorFlags(mirror));
    if (context->charge.phase == 0) {
        s16 *speed = &context->charge.speed;
        *speed -= 0x28;
        if (*speed < 0) {
            context->charge.phase = 1;
            *speed = 0;
            return;
        }
        {
            struct ContactPackedVector step;
            union SceneActorRegistryPackedPosition candidate;
            s32 cell;
            step.x =
                (*speed * gUnk_086149C4[(context->charge.motionDirection + 0x40) & 0xFF]) >> 16;
            step.z = (*speed * gUnk_086149C4[context->charge.motionDirection]) >> 16;
            candidate.position = actor->body.position;
            candidate.packed.x += step.x;
            candidate.packed.z += step.z;
            cell = SceneActorTerrain_GetCellIndex(&candidate.position);
            if (actor->terrainCell != cell) {
                context->charge.speed = 0;
                context->charge.phase = 1;
            } else {
                actor->body.motion.x += step.x;
                actor->body.motion.z += step.z;
            }
        }
    } else if (context->charge.phase == 1) {
        struct ContactPosition delta, center;
        SceneActorTerrain_GetCenter(actor->terrainCell, &center);
        SceneActorTerrain_Subtract(&delta, &center, &actor->body.position);
        switch (context->charge.motionDirection >> 6) {
            case 0:
            case 2:
                delta.z = 0;
                if (delta.x > 8)
                    body->motion.x = 8;
                else if (delta.x < -8)
                    body->motion.x = -8;
                else
                    body->motion.x = delta.x;
                break;
            case 1:
            case 3:
                delta.x = 0;
                if (delta.z > 8)
                    body->motion.z = 8;
                else if (delta.z < -8)
                    body->motion.z = -8;
                else
                    body->motion.z = delta.z;
                break;
        }
        if (delta.x + delta.z == 0) {
            context->charge.phase = 2;
            context->charge.settleFrames = 0;
            return;
        }
    } else {
        u8 *counter = &context->charge.settleFrames;
        u32 value = *counter + 1;
        u32 zero = 0;
        *counter = value;
        if ((u8)value > 15) {
            context->charge.stopPosition = context->charge.routePosition;
            sub_080C47DC(context);
            context->charge.savedDirection = context->charge.routeDirection;
            actor->body.direction = context->charge.routeDirection;
            actor->field2B0 = 1;
            *counter = zero;
            context->charge.speed = zero;
            return;
        }
    }
    actor->field2C2 = 0x40 >> (context->charge.durationSpeed >> 7);
    Golem_ApplyDuration(actor);
}
