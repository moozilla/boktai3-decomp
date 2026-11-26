#include "scene/actor/scene_actor_registry.h"
#include "engine/runtime.h"
#include "npc/manager/scene_actor_animation.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorControl_OnIdle(u8 *);
void SceneActorTerrain_DisableCollision(struct ChaseActor *);
void SceneActorRecord_RecordPair(u32, u32);
static inline u8 IsUnavailable(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    if (!actor)
        goto zero;
    {
        u32 mask;
        u32 *field;
        u32 value;
        mask = 0x40;
        value = actor->base.motionFlags;
        value &= mask;
        if (value)
            goto one;
        mask = 0x20000;
        field = &actor->base.statusFlags;
        value = *field;
        value &= mask;
        if (value)
            goto one;
    }
    if (actor->base.health > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}
static inline u8 IsScenePaused(void)
{
    if (gEngineFlags & 0x4800)
        return 1;
    return 0;
}
static inline u32 ReadSuppressionFlag(struct ChaseActor *actor)
{
    u32 mask = 0x800;
    u32 *field;
    u32 value;
    field = &actor->base.statusFlags;
    value = *field;
    value &= mask;
    return value;
}
void SceneActorRegistry_DefeatActorsFromScript(void)
{
    struct SceneActorRegistryNode *node;
    struct ChaseActor *actor;
    u32 special;
    u32 category;
    SceneActorRegistry_GetManager();
    node = gSceneActorRegistryNode;
    actor = node->actor;
    special = Script_GetKeywordValue('s', 0);
    category = Script_GetKeywordValue('k', 0);
    if (actor) {
        u32 zero = 0;
        u32 one = 1;
        const void *defaultControl = (const void *)SceneActorControl_OnIdle;
        do {
            u32 *flags;
            s16 *health;
            u32 primary;
            {
                u32 mask = 2;
                u32 *field;
                u32 value;
                field = &actor->base.behaviorFlags;
                value = *field;
                value &= mask;
                flags = field;
                primary = value;
            }
            if (!primary)
                goto ordinary;
            health = &actor->base.health;
            if (*health <= 0)
                goto ordinary;
            {
                if (category && category != actor->base.category)
                    goto next;
                *health = zero;
                actor->base.attachmentTimer = zero;
                actor->base.body.position.x = zero;
                actor->base.body.position.y = 30000;
                actor->base.body.position.z = zero;
                {
                    u32 mask = 1;
                    if (!actor->base.drawableKind) {
                        struct SceneActorRegistrySequenceDrawable *drawable = actor->base.drawable;
                        u32 value = drawable->flags;
                        value |= mask;
                        drawable->flags = value;
                    } else {
                        struct AnimSprite *drawable =
                            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile;
                        u32 value = drawable->flags;
                        value |= mask;
                        drawable->flags = value;
                    }
                }

                {
                    u32 code = 16;
                    actor->base.field2B2 = one;
                    actor->base.stepDone = zero;
                    actor->base.controlKind = code;
                    actor->base.target = defaultControl;
                    actor->base.field2C8 = zero;
                    {
                        u32 *flags114 = &actor->base.pendingControlFlags;
                        u32 mask = ~1;
                        u32 value = *flags114;
                        value &= mask;
                        *flags114 = value;
                    }
                    actor->base.pendingControl = zero;
                    {
                        u32 state = 3;
                        u8 *field = &actor->base.state2A8;
                        *field = state;
                    }
                    actor->base.phase2A9 = zero;
                    actor->base.behavior2AA = zero;
                    actor->base.counter2AC = zero;
                    actor->base.field2B1 = one;
                }
                SceneActorTerrain_DisableCollision(actor);
                if (!IsScenePaused() && !ReadSuppressionFlag(actor))
                    SceneActorRecord_RecordPair(gScriptEntryProcedure, (u16)actor->base.body.id);
            }
            goto next;
        ordinary: {
            u32 unavailable = IsUnavailable(actor);
            if (!unavailable && (!category || category == actor->base.category)) {
                actor->base.health = zero;
                actor->base.attachmentTimer = zero;
                {
                    u32 mask = 0x1000;
                    u32 value;
                    value = *flags;
                    value |= mask;
                    *flags = value;
                }
                {
                    u32 specialCopy = special;
                    if (specialCopy) {
                        u32 *field;
                        u32 value;
                        specialCopy = 0x400000;
                        field = &actor->base.statusFlags;
                        value = *field;
                        value |= specialCopy;
                        *field = value;
                    }
                }
            }
        }
        next:
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
}