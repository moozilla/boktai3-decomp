#include "scene_actor_registry.h"
struct SceneActorScheduleView {
    u8 unknown0[0x3CC];
    u8 ticks;
};
s32 SceneActorRegistry_CountAvailable(void);
s32 Div(s32, s32);
static inline u8 Inactive(struct ChaseActor *actor)
{
    {
        u32 flags = actor->base.motionFlags;
        flags &= 0x40;
        if (flags)
            return 1;
    }
    if (actor->base.statusFlags & 0x20000)
        return 1;
    if (actor->base.health <= 0)
        return 1;
    return 0;
}
void SceneActorRegistry_ScheduleAvailableActors(void)
{
    s32 count = SceneActorRegistry_CountAvailable();
    if (count > 0) {
        struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
        struct ChaseActor *actor = node->actor;
        s32 selected = 0;
        s32 quota;
        s32 delay;
        if (count <= 4) {
            quota = 1;
            delay = count - 1;
        } else if (count <= 9) {
            {
                quota = 2;
            }
            delay = Div(count + 1, 2) - 1;
        } else {
            {
                quota = 4;
            }
            delay = Div(count + 3, 4) - 1;
        }
        if (actor) {
            u32 blocked;
            u32 tickOffset;
            u32 zero;
            u32 request;
            {
                blocked = 3;
            }
            tickOffset = (u32) & ((struct SceneActorScheduleView *)0)->ticks;
            {
                zero = 0;
            }
            {
                request = 0x4000;
            }
            do {
                if (Inactive(actor) || actor->base.behaviorFlags & blocked) {
                    u8 *ticks = (u8 *)actor + tickOffset;
                    *ticks = zero;
                } else {
                    u8 *ticks = (u8 *)actor + tickOffset;
                    if (*ticks)
                        (*ticks)--;
                    else if (selected < quota) {
                        *ticks = delay;
                        {
                            u16 *field = &actor->base.reactionFlags;
                            *field = (u16)request | *field;
                        }
                        selected++;
                    }
                }
                node = node->next;
                actor = node->actor;
            } while (actor);
        }
    }
}
