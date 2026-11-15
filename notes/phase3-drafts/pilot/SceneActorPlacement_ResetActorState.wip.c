#include "scene_actor_reset.h"
#include "scene_actor_init_actor.h"
static inline u32 IsDown(struct SceneActorRegistryResetView *actor, u32 motion)
{
    if (!actor)
        return 0;
    if ((motion & 0x40) || (actor->flags2D0 & 0x20000) || actor->health <= 0)
        return 1;
    return 0;
}
void SceneActorPlacement_ResetActorState(struct SceneActorRegistryResetView *actor,
                                         const u16 *argument)
{
    u16 *field = &actor->flags2D4;
    u32 motion = *field;
    u32 mask = 0x800;
    if (mask & motion)
        return;
    motion |= mask;
    *field = motion;
    switch (*argument) {
        case 0:
            if (!(actor->flags2CC & 1))
                return;
            actor->flags2CC &= ~3;
            SceneActorRegistry_ResetRouteAndColliders((void *)actor);
            MonsterInit_SetState(actor, 3, 0, 0);
            MonsterInit_SetControl(actor, actor->field220, 12);
            break;
        case 1:
            if (IsDown(actor, motion))
                return;
            actor->flags2CC &= ~3;
            SceneActorRegistry_ResetRoute((void *)actor);
            MonsterInit_SetState(actor, 3, 0, 0);
            MonsterInit_SetControl(actor, actor->field220, 12);
            break;
        default:
            if (actor->flags2CC & 1) {
                SceneActorRegistry_RestoreActorDefaults((void *)actor);
                actor->colliderA.responseValue = 0;
            }
            break;
    }
}
