#include "scene_actor_terrain_reaction.h"
#include "scene_actor_registry.h"
struct SceneActorRegistryActor *SceneActorRegistry_FindById(u32);
static inline u8 HasMovementFlag(struct SceneActorRegistryActor *actor)
{
    u32 mask = 0x100;
    if (actor->flags2C0 & mask)
        return 1;
    return 0;
}
void SceneActorControl_OnIdle(u8 *);
void SceneActorRegistry_RequestScriptState(void)
{
    u32 id = Script_GetKeywordValue('n', 0);
    u32 preserveControl = Script_GetKeywordValue('m', 0);
    if (id) {
        struct SceneActorRegistryActor *actor = SceneActorRegistry_FindById(id);
        if (actor && HasMovementFlag(actor)) {
            SceneActorTerrain_EnterState((void *)actor, 2, 0, 0);
            if (!preserveControl) {
                SceneActorTerrain_InstallControl((void *)actor, (void *)SceneActorControl_OnIdle,
                                                 16);
            }
            {
                u32 mask = 0x4000;
                u32 flagOffset = (u32) & ((struct SceneActorRegistryActor *)0)->flags2D4;
                u16 *flags;
                u32 value;
                flags = (void *)((u8 *)actor + flagOffset);
                value = *flags;
                mask |= value;
                *flags = mask;
            }
            {
                u32 mask = 8;
                u32 flagOffset = (u32) & ((struct SceneActorRegistryActor *)0)->flags2D0;
                u32 *flags;
                u32 value;
                flags = (void *)((u8 *)actor + flagOffset);
                value = *flags;
                value |= mask;
                *flags = value;
            }
        }
    }
}
