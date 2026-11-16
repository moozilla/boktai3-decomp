#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_turn.h"
#include "display_sequence.h"

u32 Route_GetCurrentPointFacing(void *);

static inline u16 GolemTurn_Elapsed(void *sequence)
{
    return ((struct DisplaySequenceState *)sequence)->elapsed;
}

static inline s32 GolemTurn_Half(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -((-value) >> 1);
}

/* Turn toward the route-facing octant, then restore the waiting controller. */
void sub_080C5930(struct SceneActorRegistryResetView *a)
{
    struct SceneActorChaseGroup *group = &a->group;
    struct SceneActorBody *motion = &a->base.body;
    u16 *motionFlags;
    u8 prepared = SceneActorReaction_TakePrepared(a);
    s32 direction, target, delta;

    motionFlags = &a->base.flags2D4;
    if (prepared) {
        a->base.body.direction = (SceneActorTurn_Cardinal(a->base.body.direction) << 6) - 0x40;
        direction = SceneActorTurn_Cardinal(a->base.body.direction);
        if (a->base.behavior2AA != 6 && !(SceneActorTurn_Flags(motionFlags, 0x480))) {
            target = (((u16)Route_GetCurrentPointFacing(motion->currentRoute) + 0x70) >> 5) & 7;
            a->base.unknown3CD[0] = target;
        } else {
            target = a->base.unknown3CD[0];
        }
        if (direction == GolemTurn_Half(target)) {
            a->base.field2B0 = 1;
            if (SceneActorTurn_Flags(motionFlags, 0x400)) {
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
                    SceneActorTurn_SelectAnimation(a, 2, 1, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 2, 0, 4);
                    SceneActorChase_ClearWordFlags(&a->base.flags2CC, 0x10000);
                }
                break;
            case 1:
                if ((u32)(target - 4) <= 3) {
                    SceneActorTurn_SelectAnimation(a, 2, 2, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 2, 1, 4);
                    SceneActorChase_ClearWordFlags(&a->base.flags2CC, 0x10000);
                }
                break;
            case 2:
                if ((u32)(target - 1) > 4) {
                    SceneActorTurn_SelectAnimation(a, 2, 1, 5);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 2, 2, 4);
                    SceneActorChase_ClearWordFlags(&a->base.flags2CC, 0x10000);
                }
                break;
            case 3:
                if ((u32)target <= 3) {
                    SceneActorTurn_SelectAnimation(a, 2, 0, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 2, 1, 1);
                    SceneActorChase_ClearWordFlags(&a->base.flags2CC, 0x10000);
                }
                break;
        }
        a->base.field2B3 = 0;
    }
    if (GolemTurn_Elapsed(
            ((struct SceneActorAnimationSequenceDrawable *)a->base.drawable)->sequence) == 0) {
        s32 targetDirection = SceneActorTurn_OctantDirection(a->base.unknown3CD[0]);
        s32 difference = motion->direction - targetDirection;
        if (difference < 0)
            difference = -difference;
        if (difference <= 0x16)
            motion->direction = targetDirection;
        else if (SceneActorTurn_ActorFlags(a, 0x10000))
            motion->direction += 0x16;
        else
            motion->direction -= 0x16;
    }
    SceneActorTurn_ApplyDuration(a, motionFlags);
    if (a->base.field2B3) {
        s32 current = ((a->base.body.direction + 0x70) >> 5) & 7;
        delta = a->base.unknown3CD[0] - current;
        if (delta < 0)
            delta = -delta;
        if (delta <= 0) {
            a->base.field2B0 = 1;
            a->base.body.direction = (SceneActorTurn_Cardinal(a->base.body.direction) << 6) - 0x40;
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
