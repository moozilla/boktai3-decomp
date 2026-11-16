#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_turn.h"

u32 Route_GetCurrentPointFacing(void *);

struct SkeletonTurnTile {
    struct SceneActorAnimationTile base;
    u16 elapsed;
    u8 unknownE[6];
    u16 frame;
};

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
void sub_080BAE04(struct ChaseActor *a)
{
    struct SceneActorChaseGroup *group = &a->group;
    struct SceneActorBody *motion = &a->base.body;
    struct SceneActorRegistryRoute *route = &group->homeRoute;
    u16 *motionFlags;
    u8 prepared = SceneActorReaction_TakePrepared(a);
    s32 direction, target, delta;
    const u8 *entry;
    struct SkeletonTurnTile *tile;

    motionFlags = &a->base.flags2D4;
    if (prepared) {
        a->base.body.direction = (SceneActorTurn_Cardinal(a->base.body.direction) << 6) - 0x40;
        direction = SceneActorTurn_Cardinal(a->base.body.direction);
        if (a->base.behavior2AA != 6 && a->base.behavior2AA != 0x14 &&
            !SceneActorTurn_Flags(motionFlags, 0x4480)) {
            target = (((u16)Route_GetCurrentPointFacing(route) + 0x70) >> 5) & 7;
            a->base.nextDirection = target;
        } else {
            target = a->base.nextDirection;
        }
        if (direction == (target >> 1)) {
            a->base.field2B0 = 1;
            if (SceneActorTurn_Flags(motionFlags, 0x400)) {
                SceneActorChase_SetState(a, 3, 1, 0);
                SceneActorChase_SetControl(a, group->turnControl, 0x1B);
            }
            return;
        }
        entry = SceneActorProfiles_GetEntryAddress(a->base.profileId, a->base.profileMode, 2);
        switch (direction) {
            case 0:
                if ((u32)(target - 2) <= 3) {
                    SceneActorTurn_SelectAnimation(a, *entry, 1, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, *entry, 0, 4);
                    SceneActorChase_ClearWordFlags(&a->base.flags2CC, 0x10000);
                }
                break;
            case 1:
                if ((u32)(target - 4) <= 3) {
                    SceneActorTurn_SelectAnimation(a, *entry, 2, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, *entry, 1, 4);
                    SceneActorChase_ClearWordFlags(&a->base.flags2CC, 0x10000);
                }
                break;
            case 2:
                if ((u32)(target - 1) > 4) {
                    SceneActorTurn_SelectAnimation(a, *entry, 1, 5);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, *entry, 2, 4);
                    SceneActorChase_ClearWordFlags(&a->base.flags2CC, 0x10000);
                }
                break;
            case 3:
                if ((u32)target <= 3) {
                    SceneActorTurn_SelectAnimation(a, *entry, 0, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, *entry, 1, 1);
                    SceneActorChase_ClearWordFlags(&a->base.flags2CC, 0x10000);
                }
                break;
        }
    }
    tile = (void *)&((struct SceneActorAnimationTileDrawable *)a->base.drawable)->tile;
    if (tile->elapsed == 0 && (u16)(tile->frame - 1) <= 1) {
        s32 targetDirection = SceneActorTurn_OctantDirection(a->base.nextDirection);
        s32 difference = motion->direction - targetDirection;
        if (difference < 0)
            difference = -difference;
        if (difference <= 0x20)
            motion->direction = targetDirection;
        else if (SceneActorTurn_ActorFlags(a, 0x10000))
            motion->direction += 0x20;
        else
            motion->direction -= 0x20;
    }
    SceneActorTurn_ApplyDuration(a, motionFlags);
    if (a->base.restart) {
        u8 *facing = &a->base.body.direction;
        s32 current = SkeletonTurn_Octant(*facing);
        s32 octantMask = -8;
        a->base.field2B0 = 1;
        delta = SkeletonTurn_OctantDistance(a->base.nextDirection, current | octantMask);
        if (delta < 0)
            delta = -delta;
        if (delta == 0) {
            a->base.field2B0 = 1;
            *facing = (a->base.nextDirection + 5) << 5;
            SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            if (SceneActorTurn_Flags(motionFlags, 0x400)) {
                SceneActorChase_SetState(a, 3, 1, 0);
                SceneActorChase_SetControl(a, group->turnControl, 0x1B);
            }
        } else {
            SceneActorChase_SetControl(a, a->base.entryTarget224, 2);
        }
    }
}
