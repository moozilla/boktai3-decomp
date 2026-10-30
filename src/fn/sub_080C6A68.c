#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"

extern const u16 gUnk_08606130[];
void sub_080C47B8(struct ContactPosition *);
s32 sub_080C47DC(void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);

/* The context-owned charge state spans 0x680..0x6A3. Its layout is
 * distinct from the canonical actor animation record at the same offsets. */
struct GolemChargeState {
    u16 mode;
    s16 speed;
    u16 durationSpeed, field686, field688, field68A;
    u8 quadrant, phase, routeDirection, settleFrames, savedDirection, motionDirection;
    u16 field692;
    struct ContactPosition routePosition, stopPosition;
};

struct GolemChargeContext {
    u8 unknown0[0x680];
    struct GolemChargeState charge;
};

static inline void Golem_SetMotionFlags(struct SceneActorRegistryResetView *actor, u16 mask)
{
    actor->flags2D4 = mask | actor->flags2D4;
}

static inline u32 Golem_MirrorFlags(u8 mirror)
{
    return gUnk_08606130[mirror] | mirror;
}

static inline void Golem_GetDirectionVariant(struct SceneActorRegistryResetView *actor, u8 *variant,
                                             u8 *mirror)
{
    s32 quadrant = ((actor->body.direction + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *mirror = 1;
        *variant = 3 - quadrant;
    } else {
        *mirror = 0;
        *variant = quadrant;
    }
}

static inline void Golem_SelectAnimation(struct SceneActorRegistryResetView *actor, u32 selection,
                                         u32 variant, u32 flags)
{
    u32 force;
    u8 *restart;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->animationIndex = selection;
        drawable = actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
        restart = &actor->field2B3;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *savedSelection;
        selection += variant;
        savedSelection = &actor->animationIndex;
        restart = &actor->field2B3;
        if (!force && *savedSelection == selection && !*restart) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        *savedSelection = selection;
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, (u16)mode);
            flags &= 1;
            if (flags)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}

static inline void Golem_ApplyDuration(struct SceneActorRegistryResetView *actor)
{
    u16 *scale;
    u32 mask = 0xA02F;
    if (SceneActorChase_QueryMotionFlags(actor, mask)) {
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
