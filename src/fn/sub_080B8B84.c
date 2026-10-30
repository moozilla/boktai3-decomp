// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
/* SceneActorReaction_TrySink: when a live, unblocked actor stands on a sinking cell (cell flag 0x40) at or below the ground height, hands control to its sink callback (default SceneActorReaction_SinkAndRemove, kind 0x16) in state 3 and marks it sinking (flags2D0 |= 0x20000); returns whether it did (provisional name) */
void SceneActorReaction_SinkAndRemove(void);

static inline u8 Blocked(struct SceneActorRegistryResetView *actor)
{
    u32 mask = 0x40;
    register u32 offset asm("r2") = 0x2D4;
    if (*(u16 *)((u8 *)actor + offset) & mask)
        return 1;
    mask = 0x20000;
    offset -= 4;
    {
        register u32 *field asm("r0") = (void *)((u8 *)actor + offset);
        if (*field & mask)
            return 1;
    }
    if (actor->health <= 0)
        return 1;
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state)
{
    actor->state2A8 = state;
    actor->phase2A9 = 0;
    actor->behavior2AA = 0;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

u8 SceneActorReaction_TrySink(struct SceneActorRegistryResetView *actor)
{
    if (actor && !Blocked(actor) && actor->enabled == 1) {
        u16 cellFlags = *(u16 *)(SceneActorTerrain_GetCell(actor->terrainCell) + 2);
        if (cellFlags & 0x40 &&
            actor->body.position.y + (s16)actor->field2EA <= actor->groundHeight2BE) {
            void *sink = *(void **)((u8 *)actor + 0x238);
            if (!sink)
                SetControl(actor, SceneActorReaction_SinkAndRemove, 0x16);
            else
                SetControl(actor, sink, 0x16);
            SetState(actor, 3);
            {
                u32 mask = 0x20000;
                actor->flags2D0 |= mask;
            }
            return 1;
        }
    }
    return 0;
}
