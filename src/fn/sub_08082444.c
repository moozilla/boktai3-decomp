#include "scene_actor_reaction.h"
#include "engine.h"
static inline void Turn_SetFlags(struct SceneActorRegistryResetView *actor, u16 mask)
{
    actor->flags2D4 = mask | actor->flags2D4;
}
static inline void Turn_ResetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = 0;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void Turn_SetControl(struct SceneActorRegistryResetView *actor, u32 kind,
                                   void *control)
{
    u32 *flags;
    u32 mask;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    flags = &actor->flags114;
    mask = ~1;
    *flags &= mask;
    actor->word104 = 0;
}
void SceneActorReaction_RestoreAfterTurns(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct SceneActorChaseGroup *group = &actor->group;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Turn_SetFlags(actor, 0x400);
        actor->field2C2 = 32;
    }
    if (group->turn.turnsRemaining > 0) {
        if (frame > 60) {
            /* data holds the entry control, then the random table. */
            const void *data;
            u32 encoded;
            Turn_ResetState(actor, 3, 1);
            data = actor->entryTarget224;
            Turn_SetControl(actor, 2, (void *)data);
            if (group->turn.turnsRemaining == 1) {
                encoded = ((group->turn.savedDirection >> 5) + 3) & 7;
                actor->unknown3CD[0] = encoded;
            } else {
                s32 prior = actor->body.direction;
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
                encoded = ((choice >> 5) + 3) & 7;
                actor->unknown3CD[0] = encoded;
            }
            group->turn.turnsRemaining--;
        }
    } else {
        group->turn.turnsRemaining |= -1;
        if (frame > 30) {
            struct SceneActorChaseSavedControl *saved = &group->saved;
            u32 kind = saved->kind;
            void *control = saved->control;
            u32 state, phase, behavior;
            Turn_SetControl(actor, kind, control);
            state = saved->state;
            phase = saved->phase;
            behavior = saved->behavior;
            actor->state2A8 = state;
            actor->phase2A9 = phase;
            actor->behavior2AA = behavior;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
            {
                u16 *flags = &actor->flags2D4;
                u32 mask = ~0x400;
                *flags = mask & *flags;
            }
        }
    }
}
