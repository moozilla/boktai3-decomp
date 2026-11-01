#include "global.h"
// confidence=low: neighboring Golem controls establish the state role.
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "scene_object.h"
#include "scene_actor_chase_reaction.h"
extern const u16 gUnk_08606130[];
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
struct GolemRouteMotion {
    u8 unknown[0x14];
    s16 x, y, z;
};
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
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

// confidence=low: return-home behaviors use this control while moving to a cell center.
void Golem_UpdateRouteMotion(struct SceneActorRegistryResetView *actor, u32 frame)
{
    u8 *body = actor->context;
    struct GolemRouteMotion *motion = (void *)((u8 *)actor + 0x50);
    if (TakeFlag(&actor->field2B2))
        actor->field2B3 = 0;
    Golem_SetMotionFlags(actor, 0x100);
    {
        u8 directionVariant, mirror;
        Golem_GetDirectionVariant(actor, &directionVariant, &mirror);
        Golem_SelectAnimation(actor, 8, directionVariant, Golem_MirrorFlags(mirror));
    }
    actor->field2C2 = 0x40 >> (*(s16 *)(body + 0x682) >> 7);
    Golem_ApplyDuration(actor);
    if (!(frame & 3)) {
        struct SceneExtent velocity, size;
        velocity.x = -motion->x;
        velocity.y = 0;
        velocity.z = -motion->z;
        size.x = 32;
        size.y = 0;
        size.z = 32;
        SceneParticleBurst_CreateWithPause((void *)((u8 *)motion + 0xC), (s32)&velocity, &size, 1,
                                           3, 4);
    }
}
