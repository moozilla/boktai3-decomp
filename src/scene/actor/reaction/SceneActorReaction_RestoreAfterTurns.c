#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_reaction.h"
#include "engine/engine.h"
#include "scene/motion/scene_actor_chase_shared.h"
void SceneActorReaction_RestoreAfterTurns(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        SceneActorChase_SetMotionFlags(actor, 0x400);
        actor->base.field2C2 = 32;
    }
    if (group->turn.turnsRemaining > 0) {
        if (frame > 60) {
            /* data holds the entry control, then the random table. */
            const void *data;

            SceneActorTerrain_EnterState(actor, 3, 1, 0);
            data = actor->base.onEntry;
            SceneActorTerrain_InstallControl(actor, (void *)data, 2);
            if (group->turn.turnsRemaining == 1) {
                actor->base.nextDirection = ((group->turn.savedDirection >> 5) + 3) & 7;
            } else {
                s32 prior = actor->base.body.direction;
                s32 tries = 0;
                u32 *randomIndex;
                u32 wrap, mask;
                s32 choice;
                data = gRandomTable;
                randomIndex = &gRandomIndex;
                wrap = 1023;
                mask = 3;
                for (;;) {
                    u32 index = (*randomIndex + 1) & wrap;
                    s32 next;
                    *randomIndex = index;
                    next = (((const u16 *)data)[index] & mask) << 6;
                    if (next != prior) {
                        choice = next;
                        break;
                    }
                    if (++tries > 3) {
                        choice = prior;
                        break;
                    }
                }
                actor->base.nextDirection = ((choice >> 5) + 3) & 7;
            }
            group->turn.turnsRemaining--;
        }
    } else {
        group->turn.turnsRemaining = -1;
        if (frame > 30) {
            struct SceneActorChaseSavedControl *saved = &group->saved;
            SceneActorTerrain_InstallControl(actor, saved->control, saved->kind);
            SceneActorTerrain_EnterState(actor, saved->state, saved->phase, saved->behavior);
            {
                u16 *flags = &actor->base.motionFlags;
                u32 mask = ~0x400;
                *flags = mask & *flags;
            }
        }
    }
}
