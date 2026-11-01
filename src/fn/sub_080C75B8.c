#include "global.h"
// confidence=low: neighboring Golem controls establish the state role.
/* Decelerate recoil, converge on the saved position, then signal completion. */
// confidence=low: inferred from the three movement phases and neighboring Golem controls.
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "scene_actor_chase_reaction.h"

extern const u16 gUnk_08606130[];
void sub_080C47B8(struct ContactPosition *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);

struct Unk080C75B8Motion {
    u8 unknown0[0xC];
    struct ContactPosition position;
    s16 x, unknown16, z;
};

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

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

void Golem_UpdateRecoil(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    struct Unk080C75B8Motion *motion = (struct Unk080C75B8Motion *)((u8 *)actor + 0x50);
    s16 *speed;

    if (TakeFlag(&actor->field2B2)) {
        actor->field2B3 = 0;
        body[0x68D] = 0;
        {
            u32 angle = (body[0x68E] + 0x60) >> 6;
            u32 quadrant = 3;
            quadrant &= angle;
            body[0x68C] = quadrant;
        }
        *(struct ContactPosition *)(body + 0x69C) = motion->position;
        sub_080C47B8((struct ContactPosition *)(body + 0x69C));
    }
    Golem_SetMotionFlags(actor, 0x100);
    {
        u32 mask = 0x2000;
        actor->flags2CC |= mask;
    }
    {
        u8 directionVariant, mirror;
        Golem_GetDirectionVariant(actor, &directionVariant, &mirror);
        Golem_SelectAnimation(actor, 8, directionVariant, Golem_MirrorFlags(mirror));
    }
    if (body[0x68D] == 0) {
        switch (*(u16 *)((u8 *)actor + 0x1E6)) {
            case 1:
            case 3:
            case 5:
                *(s16 *)(body + 0x682) -= 0xC0;
                break;
            case 0:
                *(s16 *)(body + 0x682) -= 0x40;
                break;
            default:
                *(s16 *)(body + 0x682) -= 0x80;
                break;
        }
        speed = (s16 *)(body + 0x682);
        if (*speed < 0) {
            u8 *phase = body + 0x68D;
            u32 zero = 0;
            *phase = 1;
            *speed = zero;
            return;
        }
        actor->body.motion.x += (*speed * gUnk_086149C4[(body[0x68E] + 0x40) & 0xFF]) >> 16;
        actor->body.motion.z += (*speed * gUnk_086149C4[body[0x68E]]) >> 16;
    } else if (body[0x68D] == 1) {
        struct ContactPosition delta;
        Delta(&delta, &motion->position, (struct ContactPosition *)(body + 0x69C));
        switch (body[0x691] >> 6) {
            case 0:
            case 2:
                delta.z = 0;
                if (delta.x > 8)
                    motion->x = 8;
                else if (delta.x < -8)
                    motion->x = -8;
                else
                    motion->x = delta.x;
                break;
            case 1:
            case 3:
                delta.x = 0;
                if (delta.z > 8)
                    motion->z = 8;
                else if (delta.z < -8)
                    motion->z = -8;
                else
                    motion->z = delta.z;
                break;
        }
        if (delta.x + delta.z == 0) {
            body[0x68D] = 2;
            body[0x68F] = 0;
            return;
        }
    } else {
        u8 *counter = body + 0x68F;
        u32 value = *counter + 1;
        u32 zero = 0;
        *counter = value;
        if ((u8)value > 7) {
            actor->field2B0 = 1;
            *counter = zero;
            *(u16 *)(body + 0x682) = zero;
            return;
        }
    }
    actor->field2C2 = 0x40 >> (*(u16 *)(body + 0x684) >> 7);
    Golem_ApplyDuration(actor);
}
