#include "scene_actor_registry.h"
#include "runtime.h"
#include "scene_actor_animation.h"
struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void);
void SceneActorControl_OnIdle(u8 *);
void SceneActorTerrain_DisableCollision(struct SceneActorRegistryActor *);
void SceneActorRecord_RecordPair(u32, u32);
static inline u8 IsUnavailable(struct SceneActorRegistryActor *input)
{
    struct SceneActorRegistryActor *actor = input;
    if (!actor)
        goto zero;
    {
        u32 mask;
        u32 *field;
        u32 value;
        mask = 0x40;
        value = actor->flags2D4;
        value &= mask;
        if (value)
            goto one;
        mask = 0x20000;
        field = &actor->flags2D0;
        value = *field;
        value &= mask;
        if (value)
            goto one;
    }
    if (actor->health > 0)
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
static inline u32 ReadSuppressionFlag(struct SceneActorRegistryActor *actor)
{
    u32 mask = 0x800;
    u32 *field;
    u32 value;
    field = &actor->flags2D0;
    value = *field;
    value &= mask;
    return value;
}
void SceneActorRegistry_DefeatActorsFromScript(void)
{
    struct SceneActorRegistryNode *node;
    struct SceneActorRegistryActor *actor;
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
                field = &actor->flags2CC;
                value = *field;
                value &= mask;
                flags = field;
                primary = value;
            }
            if (!primary)
                goto ordinary;
            health = &actor->health;
            if (*health <= 0)
                goto ordinary;
            {
                if (category && category != actor->category)
                    goto next;
                *health = zero;
                actor->fieldAA = zero;
                actor->position.x = zero;
                actor->position.y = 30000;
                actor->position.z = zero;
                {
                    u32 mask = 1;
                    if (!actor->drawableKind) {
                        struct SceneActorRegistrySequenceDrawable *drawable = actor->drawable;
                        u32 value = drawable->flags;
                        value |= mask;
                        drawable->flags = value;
                    } else {
                        struct SceneActorAnimationTile *drawable =
                            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
                        u32 value = drawable->flags;
                        value |= mask;
                        drawable->flags = value;
                    }
                }

                {
                    u32 code = 16;
                    actor->field2B2 = one;
                    actor->field2B0 = zero;
                    actor->unknown2B5[0] = code;
                    actor->control = defaultControl;
                    actor->field2C8 = zero;
                    {
                        u32 *flags114 = &actor->flags114;
                        u32 mask = ~1;
                        u32 value = *flags114;
                        value &= mask;
                        *flags114 = value;
                    }
                    actor->field104 = zero;
                    {
                        u32 state = 3;
                        u8 *field = &actor->state2A8;
                        *field = state;
                    }
                    actor->phase2A9 = zero;
                    actor->behavior2AA = zero;
                    actor->counter2AC = zero;
                    actor->field2B1 = one;
                }
                SceneActorTerrain_DisableCollision(actor);
                if (!IsScenePaused() && !ReadSuppressionFlag(actor))
                    SceneActorRecord_RecordPair(gScriptEntryProcedure, (u16)actor->id);
            }
            goto next;
        ordinary: {
            u32 unavailable = IsUnavailable(actor);
            if (!unavailable && (!category || category == actor->category)) {
                actor->health = zero;
                actor->fieldAA = zero;
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
                        field = &actor->flags2D0;
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