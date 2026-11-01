// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
s32 SceneActorRegistry_CountAvailable(void);
s32 Div(s32, s32);
static inline u8 Inactive(struct SceneActorRegistryActor *actor)
{
    {
        u32 flags = actor->flags2D4;
        flags &= 0x40;
        if (flags)
            return 1;
    }
    if (actor->flags2D0 & 0x20000)
        return 1;
    if (actor->health <= 0)
        return 1;
    return 0;
}
void SceneActorRegistry_ScheduleAvailableActors(void)
{
    s32 count = SceneActorRegistry_CountAvailable();
    if (count > 0) {
        struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
        struct SceneActorRegistryActor *actor = node->actor;
        s32 selected = 0;
        s32 quota;
        s32 delay;
        if (count <= 4) {
            quota = 1;
            delay = count - 1;
        } else if (count <= 9) {
            {
                s32 value = 2;
                quota = value;
            }
            delay = Div(count + 1, 2) - 1;
        } else {
            {
                s32 value = 4;
                quota = value;
            }
            delay = Div(count + 3, 4) - 1;
        }
        if (actor) {
            u32 blocked;
            u32 tickOffset;
            u32 zero;
            register u32 request asm("r9");
            {
                u32 low = 3;
                blocked = low;
            }
            tickOffset = 0x3CC;
            {
                u32 low = 0;
                zero = low;
            }
            {
                u32 low = 0x4000;
                request = low;
            }
            do {
                if (Inactive(actor) || actor->flags2CC & blocked) {
                    u8 *ticks = (u8 *)actor + tickOffset;
                    *ticks = zero;
                } else {
                    u8 *ticks = (u8 *)actor + tickOffset;
                    if (*ticks)
                        (*ticks)--;
                    else if (selected < quota) {
                        *ticks = delay;
                        {
                            u16 *field = &actor->flags2D6;
                            u32 flags = *field;
                            register u32 mask asm("r0");
                            mask = request;
                            asm("" : "+r"(mask));
                            mask |= flags;
                            *field = mask;
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
