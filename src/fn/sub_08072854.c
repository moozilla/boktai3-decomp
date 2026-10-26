#include "global.h"

#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"

// confidence=low: walks the registered actor list; for each enabled actor outside the excluded categories, a sinking cell hands it to its sink callback, and a cell with bit 0x80 (while the global flag 0x10 is set and actor+0x178 is 0) aims it at the cell centre
void SceneActorReaction_SinkAndRemove(void);

static inline u8 Sub08072854_Excluded(u8 category)
{
    u32 c = category - 0x11;
    if (c <= 3 || category == 0x17)
        return 1;
    return 0;
}

static inline u8 Sub08072854_Blocked(struct SceneActorRegistryResetView *actor)
{
    if (actor->flags2D4 & 0x40)
        return 1;
    if (actor->flags2D0 & 0x20000)
        return 1;
    if (actor->health <= 0)
        return 1;
    return 0;
}

static inline void Sub08072854_Sink(struct SceneActorRegistryResetView *actor, void *sink)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = 0x16;
    actor->target = sink;
    actor->field2C8 = 0;
    actor->flags114 &= ~1;
    actor->word104 = 0;
    actor->state2A8 = 3;
    actor->phase2A9 = 0;
    actor->behavior2AA = 0;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
    actor->flags2D0 |= 0x20000;
}

#ifdef NONMATCHING
// fuzzy: 55.9%
// scope shape 115 (differences once low registers are set aside)
// confidence=low: walks the registered actor list; for each enabled actor outside the excluded categories, a sinking cell hands it to its sink callback, and a cell with bit 0x80 (while the global flag 0x10 is set and actor+0x178 is 0) aims it at the cell centre
void sub_08072854(void)
{
    struct SceneActorRegistryNode *node = gUnk_020004AC;
    struct SceneActorRegistryResetView *actor;

    if (!node)
        return;
    actor = (struct SceneActorRegistryResetView *)node->actor;
    while (actor) {
        if (actor->enabled == 1 && !Sub08072854_Excluded(actor->category)) {
            u16 cellFlags = *(u16 *)(SceneActorTerrain_GetCell(actor->terrainCell) + 2);

            if ((cellFlags & 0x40) &&
                actor->position.y + (s16)actor->field2EA <= actor->groundHeight2BE) {
                if (!Sub08072854_Blocked(actor)) {
                    void *sink = *(void **)((u8 *)actor + 0x238);
                    if (!sink)
                        sink = SceneActorReaction_SinkAndRemove;
                    Sub08072854_Sink(actor, sink);
                }
            } else if ((cellFlags & 0x80) && (*(u32 *)((u8 *)gGameState + 0x868) & 0x10) &&
                       *(u16 *)((u8 *)actor + 0x178) == 0) {
                struct ContactPosition center, delta;
                u8 *collider = (u8 *)actor + 0x134;

                SceneActorTerrain_GetCenter(actor->terrainCell, &center);
                SceneActorTerrain_Subtract(&delta, &center, &actor->position);
                actor->field2B6 = Math_Atan2(delta.x, delta.z) >> 5;
                actor->field2E0 = 0x20;
                *(s16 *)(collider + 0x3E) = actor->health;
                *(s16 *)((u8 *)actor + 0x2E4) = actor->health;
                *(u16 *)(collider + 0x40) = 0x28;
                actor->field2D8 = 0x200;
                actor->field2E6 = 1;
                actor->flags2CC |= 0x2000;
            }
        }
        node = node->next;
        actor = (struct SceneActorRegistryResetView *)node->actor;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08072854);
#endif
