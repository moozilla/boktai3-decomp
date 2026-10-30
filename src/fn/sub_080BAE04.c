#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"

u32 Route_GetCurrentPointFacing(void *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);

static inline s32 SkeletonTurn_Cardinal(u8 direction)
{
    return ((direction + 0x60) >> 6) & 3;
}
static inline u32 SkeletonTurn_ActorFlags(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2CC & mask;
}
static inline void SkeletonTurn_ApplyDuration(struct SceneActorRegistryResetView *a,
                                              u16 *motionFlags)
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
static inline u32 SkeletonTurn_Flags(u16 *field, u32 mask)
{
    return *field & mask;
}
static inline void SkeletonTurn_SetAnimationIndex(struct SceneActorRegistryResetView *a,
                                                  u32 selection)
{
    a->animationIndex = selection;
}

static inline void SkeletonTurn_SelectAnimation(struct SceneActorRegistryResetView *a,
                                                u32 selection, u32 variant, u32 flags)
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
        SkeletonTurn_SetAnimationIndex(a, selection);
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

struct SkeletonTurnTile {
    struct SceneActorAnimationTile base;
    u16 elapsed;
    u8 unknownE[6];
    u16 frame;
};

static inline u8 SkeletonTurn_OctantDirection(u32 octant)
{
    return ((octant + 5) & 7) << 5;
}

static inline s32 SkeletonTurn_Octant(s16 direction)
{
    return (direction + 0x70) >> 5;
}

static inline s32 SkeletonTurn_OctantDistance(s32 target, s32 current)
{
    s32 delta = (target - current) & 7;
    if (delta > 4)
        delta = 8 - delta;
    return delta;
}

/* Turn toward the route octant with the profile-selected animation. */
void sub_080BAE04(struct SceneActorRegistryResetView *a)
{
    struct SceneActorChaseGroup *group = &a->group;
    struct SceneActorBody *motion = &a->body;
    struct SceneActorRegistryRoute *route = &group->homeRoute;
    u16 *motionFlags;
    u8 prepared = SceneActorReaction_TakePrepared(a);
    s32 direction, target, delta;
    const u8 *entry;
    struct SkeletonTurnTile *tile;

    motionFlags = &a->flags2D4;
    if (prepared) {
        a->body.direction = (SkeletonTurn_Cardinal(a->body.direction) << 6) - 0x40;
        direction = SkeletonTurn_Cardinal(a->body.direction);
        if (a->behavior2AA != 6 && a->behavior2AA != 0x14 &&
            !SkeletonTurn_Flags(motionFlags, 0x4480)) {
            target = (((u16)Route_GetCurrentPointFacing(route) + 0x70) >> 5) & 7;
            a->unknown3CD[0] = target;
        } else {
            target = a->unknown3CD[0];
        }
        if (direction == (target >> 1)) {
            a->field2B0 = 1;
            if (SkeletonTurn_Flags(motionFlags, 0x400)) {
                SceneActorChase_SetState(a, 3, 1, 0);
                SceneActorChase_SetControl(a, group->turnControl, 0x1B);
            }
            return;
        }
        entry = SceneActorProfiles_GetEntryAddress(a->profileId, a->profileMode, 2);
        switch (direction) {
            case 0:
                if ((u32)(target - 2) <= 3) {
                    SkeletonTurn_SelectAnimation(a, *entry, 1, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SkeletonTurn_SelectAnimation(a, *entry, 0, 4);
                    SceneActorChase_ClearWordFlags(&a->flags2CC, 0x10000);
                }
                break;
            case 1:
                if ((u32)(target - 4) <= 3) {
                    SkeletonTurn_SelectAnimation(a, *entry, 2, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SkeletonTurn_SelectAnimation(a, *entry, 1, 4);
                    SceneActorChase_ClearWordFlags(&a->flags2CC, 0x10000);
                }
                break;
            case 2:
                if ((u32)(target - 1) > 4) {
                    SkeletonTurn_SelectAnimation(a, *entry, 1, 5);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SkeletonTurn_SelectAnimation(a, *entry, 2, 4);
                    SceneActorChase_ClearWordFlags(&a->flags2CC, 0x10000);
                }
                break;
            case 3:
                if ((u32)target <= 3) {
                    SkeletonTurn_SelectAnimation(a, *entry, 0, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SkeletonTurn_SelectAnimation(a, *entry, 1, 1);
                    SceneActorChase_ClearWordFlags(&a->flags2CC, 0x10000);
                }
                break;
        }
    }
    tile = (void *)&((struct SceneActorAnimationTileDrawable *)a->drawable)->tile;
    if (tile->elapsed == 0 && (u16)(tile->frame - 1) <= 1) {
        s32 targetDirection = SkeletonTurn_OctantDirection(a->unknown3CD[0]);
        s32 difference = motion->direction - targetDirection;
        if (difference < 0)
            difference = -difference;
        if (difference <= 0x20)
            motion->direction = targetDirection;
        else if (SkeletonTurn_ActorFlags(a, 0x10000))
            motion->direction += 0x20;
        else
            motion->direction -= 0x20;
    }
    SkeletonTurn_ApplyDuration(a, motionFlags);
    if (a->field2B3) {
        u8 *facing = &a->body.direction;
        s32 current = SkeletonTurn_Octant(*facing);
        s32 octantMask = -8;
        a->field2B0 = 1;
        delta = SkeletonTurn_OctantDistance(a->unknown3CD[0], current | octantMask);
        if (delta < 0)
            delta = -delta;
        if (delta == 0) {
            a->field2B0 = 1;
            *facing = (a->unknown3CD[0] + 5) << 5;
            SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            if (SkeletonTurn_Flags(motionFlags, 0x400)) {
                SceneActorChase_SetState(a, 3, 1, 0);
                SceneActorChase_SetControl(a, group->turnControl, 0x1B);
            }
        } else {
            SceneActorChase_SetControl(a, a->entryTarget224, 2);
        }
    }
}
