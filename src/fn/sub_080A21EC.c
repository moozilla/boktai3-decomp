#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_turn.h"
#include "display_sequence.h"

static inline u8 Spider_TargetDirection(struct SceneActorRegistryResetView *actor)
{
    return ((actor->unknown3CD[0] + 5) & 7) << 5;
}

static inline void Spider_StepDirection(struct SceneActorBody *body, s32 target, s32 step)
{
    s32 current = body->direction;
    s32 difference = current - target;
    s32 magnitude = difference;
    if (magnitude < 0)
        magnitude = -magnitude;
    if (magnitude <= step)
        body->direction = target;
    else if (current < target) {
        s32 distance = difference;
        if (distance < 0)
            distance = -distance;
        if (distance <= 128)
            body->direction = current + step;
        else
            body->direction = current - step;
    } else if (current > target) {
        if (difference < 0)
            difference = -difference;
        if (difference <= 128)
            body->direction = current - step;
        else
            body->direction = current + step;
    }
}

static inline void Spider_ApplyDuration(struct SceneActorRegistryResetView *actor)
{
    u16 *scale;
    u32 mask = 0xA02F;
    if (actor->flags2D4 & mask) {
        u16 *p = &actor->field2C2;
        *p = 64;
        scale = p;
    } else {
        u16 *q = &actor->field2C2;
        u16 value = *q;
        scale = q;
        if (!value)
            *scale = 64;
    }
    if (!actor->drawableKind)
        Anim_SetDurationScale(
            ((struct SceneActorAnimationSequenceDrawable *)actor->drawable)->sequence, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile, *scale);
}

static inline u16 Spider_ReadCardinal(u8 direction)
{
    return ((direction + 0x60) >> 6) & 3;
}
void Spider_AdvanceTurnAnimation(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorBody *body = &actor->body;
    struct DisplaySequenceState *sequence;
    s32 direction, facing, difference;
    if (SceneActorReaction_TakePrepared(actor)) {
        body->direction = (SceneActorTurn_Cardinal(body->direction) << 6) - 64;
        direction = ((body->direction + 0x60) >> 6) & 3;
        facing = actor->unknown3CD[0];
        facing >>= 1;
        if (direction != facing) {
            difference = facing - direction;
            if (difference < 0)
                difference = -difference;
            if (difference == 2)
                goto turn;
        }
        actor->field2B0 = 1;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        return;
    turn:
        switch (direction) {
            case 0:
                SceneActorTurn_SelectAnimation(actor, 5, 0, 0);
                break;
            case 1:
                SceneActorTurn_SelectAnimation(actor, 5, 1, 0);
                break;
            case 2:
                SceneActorTurn_SelectAnimation(actor, 5, 0, 4);
                break;
            case 3:
                SceneActorTurn_SelectAnimation(actor, 5, 1, 4);
                break;
        }
    }
    sequence = ((struct SceneActorAnimationSequenceDrawable *)actor->drawable)->sequence;
    if (sequence->elapsed == 0) {
        s32 target = Spider_TargetDirection(actor);
        Spider_StepDirection(body, target, 64);
    }
    Spider_ApplyDuration(actor);
    if (actor->field2B3) {
        actor->field2B0 = 1;
        actor->body.direction = (Spider_ReadCardinal(actor->body.direction) << 6) - 64;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    }
}
