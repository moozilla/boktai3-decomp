#include "scene_actor_reset.h"
#include "scene_actor_init_actor.h"
struct MotionFlags {
    s16 bits : 16;
};
static inline void EnableMotionFlags(u32 mask, u16 *field)
{
    struct MotionFlags *flags = (void *)field;
    flags->bits |= mask;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
static inline void ClearFlags(u32 *flags, u32 bits)
{
    u32 mask = ~bits;
    *flags &= mask;
}
static inline u8 IsDown(struct SceneActorRegistryResetView *actor, u16 *motion)
{
    if (!actor)
        return 0;
    if (*motion & 0x40)
        return 1;
    {
        u32 mask = 0x20000;
        if (actor->base.flags2D0 & mask)
            return 1;
    }
    if (actor->base.health <= 0)
        return 1;
    return 0;
}
void SceneActorTarget_ApplyResetCommand(struct SceneActorRegistryResetView *actor,
                                         const u16 *argument)
{
    u32 mask = 0x800;
    u16 *flags = &actor->base.flags2D4;
    u16 prior = mask & *flags;
    if (prior)
        return;
    EnableMotionFlags(mask, flags);
    switch (*argument) {
        case 0:
            {
                u32 bit = 1;
                if (!(actor->base.flags2CC & bit))
                    return;
            }
                ClearFlags(&actor->base.flags2CC, 3);
                SceneActorRegistry_ResetRouteAndColliders((void *)actor);
                MonsterInit_SetState(actor, 3, 0, 0);
                SetControl(actor, actor->base.field220, 12);
                break;
        case 1:
            if (IsDown(actor, flags))
                return;
            ClearFlags(&actor->base.flags2CC, 3);
            SceneActorRegistry_ResetRoute((void *)actor);
            MonsterInit_SetState(actor, 3, 0, 0);
            SetControl(actor, actor->base.field220, 12);
            break;
        default:
            {
                u32 bit = 1;
                if (actor->base.flags2CC & bit) {
                    SceneActorRegistry_RestoreActorDefaults((void *)actor);
                    actor->base.colliderA.responseValue = 0;
                }
            }
            break;
    }
}
