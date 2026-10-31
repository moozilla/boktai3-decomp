#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_turn.h"
#include "display_sequence.h"

u32 Route_GetCurrentPointFacing(void *);

/* Turn toward the route-facing octant, then restore the waiting controller. */
void Ghoul_TurnTowardRoute(struct SceneActorRegistryResetView *a)
{
    struct SceneActorBody *motion = &a->body;
    struct SceneActorRegistryRoute *route = &a->group.homeRoute;
    struct SceneActorChaseGroup *group = &a->group;
    struct DisplaySequenceState *sequence;
    u16 *motionFlags;
    u8 prepared = SceneActorReaction_TakePrepared(a);
    s32 direction, target, delta;

    motionFlags = &a->flags2D4;
    if (prepared) {
        a->body.direction = (SceneActorTurn_Cardinal(a->body.direction) << 6) - 0x40;
        direction = SceneActorTurn_Cardinal(a->body.direction);
        if (a->behavior2AA != 6 && a->behavior2AA != 17 &&
            !SceneActorTurn_Flags(motionFlags, 0x4480)) {
            target = (((u16)Route_GetCurrentPointFacing(route) + 0x70) >> 5) & 7;
            a->unknown3CD[0] = target;
        } else {
            target = a->unknown3CD[0];
        }
        if (direction == (target >> 1)) {
            a->field2B0 = 1;
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
                    SceneActorTurn_SelectAnimation(a, 9, 1, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 9, 0, 4);
                    SceneActorChase_ClearWordFlags(&a->flags2CC, 0x10000);
                }
                break;
            case 1:
                if ((u32)(target - 4) <= 3) {
                    SceneActorTurn_SelectAnimation(a, 9, 2, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 9, 1, 4);
                    SceneActorChase_ClearWordFlags(&a->flags2CC, 0x10000);
                }
                break;
            case 2:
                if ((u32)(target - 1) > 4) {
                    SceneActorTurn_SelectAnimation(a, 9, 1, 5);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 9, 2, 4);
                    SceneActorChase_ClearWordFlags(&a->flags2CC, 0x10000);
                }
                break;
            case 3:
                if ((u32)target <= 3) {
                    SceneActorTurn_SelectAnimation(a, 9, 0, 0);
                    SceneActorChase_SetActorFlags(a, 0x10000);
                } else {
                    SceneActorTurn_SelectAnimation(a, 9, 1, 1);
                    SceneActorChase_ClearWordFlags(&a->flags2CC, 0x10000);
                }
                break;
        }
        a->field2B3 = 0;
    }
    sequence = ((struct SceneActorAnimationSequenceDrawable *)a->drawable)->sequence;
    if (sequence->elapsed == 0 && (sequence->index == 1 || sequence->index == 3)) {
        s32 targetDirection = SceneActorTurn_OctantDirection(a->unknown3CD[0]);
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
    if (a->field2B3) {
        s32 current = ((a->body.direction + 0x70) >> 5) & 7;
        delta = a->unknown3CD[0] - current;
        if (delta < 0)
            delta = -delta;
        if (delta <= 0) {
            a->field2B0 = 1;
            a->body.direction = (SceneActorTurn_Cardinal(a->body.direction) << 6) - 0x40;
            SceneActorAnimation_SelectDirectionalEntry((void *)a, 0);
            if (SceneActorTurn_Flags(motionFlags, 0x400)) {
                SceneActorChase_SetState(a, 3, 1, 0);
                SceneActorChase_SetControl(a, group->turnControl, 0x1B);
            }
        } else {
            SceneActorChase_SetControl(a, a->entryTarget224, 2);
        }
    }
}
