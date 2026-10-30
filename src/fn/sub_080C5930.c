#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "display_sequence.h"

u32 Route_GetCurrentPointFacing(void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);

static inline s32 GolemTurn_Cardinal(u8 direction)
{
    return ((direction + 0x60) >> 6) & 3;
}
static inline u16 GolemTurn_Elapsed(void *sequence)
{
    return ((struct DisplaySequenceState *)sequence)->elapsed;
}
static inline u32 GolemTurn_ActorFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2CC & mask;
}
static inline void GolemTurn_ApplyDuration(struct SceneActorRegistryResetView *a, u16 *motionFlags)
{
    u16 *scale;
    u32 mask = 0xA02F;
    if (*motionFlags & mask) {
        u16 *p = &a->field2C2;
        *p = 64;
        scale = p;
    } else {
        u16 *q = &a->field2C2;
        u16 value = *q;
        scale = q;
        if (!value)
            *scale = 64;
    }
    if (!a->drawableKind)
        Anim_SetDurationScale(((struct SceneActorAnimationSequenceDrawable *)a->drawable)->sequence,
                              *scale);
    else
        Anim_SetFrameDurationScale(&((struct SceneActorAnimationTileDrawable *)a->drawable)->tile,
                                   *scale);
}
static inline s32 GolemTurn_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -((-value) >> 1);
}
static inline u32 GolemTurn_Flags(u16 *field, u32 mask)
{
    return *field & mask;
}
static inline void GolemTurn_SetAnimationIndex(struct SceneActorRegistryResetView *a, u32 selection)
{
    a->animationIndex = selection;
}

static inline void GolemTurn_SelectAnimation(struct SceneActorRegistryResetView *a, u32 selection,
                                             u32 variant, u32 flags)
{
    u32 force;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!a->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        a->animationIndex = selection;
        drawable = a->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)a->drawable)->tile;
        selection += variant;
        {
            u8 *selectionAddress = &a->animationIndex;
            u32 previous = *selectionAddress;
            if (!force && previous == selection && !a->field2B3) {
                if (flags == 1 && (tile->flags & 4))
                    return;
                if (flags == 0 && !(tile->flags & 4))
                    return;
            }
        }
        GolemTurn_SetAnimationIndex(a, selection);
        {
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)a->drawable)->tile,
                              a->drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                tile->flags |= mask;
            else
                tile->flags &= ~4;
        }
    }
    a->field2B3 = 0;
}

static inline u8 GolemTurn_OctantDirection(u32 octant)
{
    return ((octant + 5) & 7) << 5;
}

/* Turn toward the route-facing octant, then restore the waiting controller. */
void sub_080C5930(struct SceneActorRegistryResetView *a)
{
    struct SceneActorChaseGroup *group = &a->group;
    struct SceneActorBody *motion = &a->body;
    u16 *motionFlags;
    u8 prepared = SceneActorReaction_TakePrepared(a);
    s32 direction, target, delta;

    motionFlags = &a->flags2D4;
    if (prepared) {
        a->body.direction = (GolemTurn_Cardinal(a->body.direction) << 6) - 0x40;
        direction = GolemTurn_Cardinal(a->body.direction);
        if (a->behavior2AA != 6 && !(GolemTurn_Flags(motionFlags, 0x480))) {
            target = (((u16)Route_GetCurrentPointFacing(motion->currentRoute) + 0x70) >> 5) & 7;
            a->unknown3CD[0] = target;
        } else {
            target = a->unknown3CD[0];
        }
        if (direction == GolemTurn_Half(target)) {
            a->field2B0 = 1;
            if (GolemTurn_Flags(motionFlags, 0x400)) {
                SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
                SceneActorChase_SetState(a, 3, 1, 0);
                SceneActorChase_SetControl(a, group->turnControl, 0x1B);
            } else {
                SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            }
            return;
        }
        switch (direction) {
            case 0:
                if ((u32)(target - 2) <= 3) {
                    GolemTurn_SelectAnimation(a, 2, 1, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    GolemTurn_SelectAnimation(a, 2, 0, 4);
                    SceneActorChase_ClearWordFlags(&a->flags2CC, 0x10000);
                }
                break;
            case 1:
                if ((u32)(target - 4) <= 3) {
                    GolemTurn_SelectAnimation(a, 2, 2, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    GolemTurn_SelectAnimation(a, 2, 1, 4);
                    SceneActorChase_ClearWordFlags(&a->flags2CC, 0x10000);
                }
                break;
            case 2:
                if ((u32)(target - 1) > 4) {
                    GolemTurn_SelectAnimation(a, 2, 1, 5);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    GolemTurn_SelectAnimation(a, 2, 2, 4);
                    SceneActorChase_ClearWordFlags(&a->flags2CC, 0x10000);
                }
                break;
            case 3:
                if ((u32)target <= 3) {
                    GolemTurn_SelectAnimation(a, 2, 0, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    GolemTurn_SelectAnimation(a, 2, 1, 1);
                    SceneActorChase_ClearWordFlags(&a->flags2CC, 0x10000);
                }
                break;
        }
        a->field2B3 = 0;
    }
    if (GolemTurn_Elapsed(((struct SceneActorAnimationSequenceDrawable *)a->drawable)->sequence) ==
        0) {
        s32 targetDirection = GolemTurn_OctantDirection(a->unknown3CD[0]);
        s32 difference = motion->direction - targetDirection;
        if (difference < 0)
            difference = -difference;
        if (difference <= 0x16)
            motion->direction = targetDirection;
        else if (GolemTurn_ActorFlags(a, 0x10000))
            motion->direction += 0x16;
        else
            motion->direction -= 0x16;
    }
    GolemTurn_ApplyDuration(a, motionFlags);
    if (a->field2B3) {
        s32 current = ((a->body.direction + 0x70) >> 5) & 7;
        delta = a->unknown3CD[0] - current;
        if (delta < 0)
            delta = -delta;
        if (delta <= 0) {
            a->field2B0 = 1;
            a->body.direction = (GolemTurn_Cardinal(a->body.direction) << 6) - 0x40;
            SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            if (GolemTurn_Flags(motionFlags, 0x400)) {
                SceneActorChase_SetState(a, 3, 1, 0);
                SceneActorChase_SetControl(a, group->turnControl, 0x1B);
            }
        } else {
            SceneActorChase_SetControl(a, a->entryTarget224, 2);
        }
    }
}
