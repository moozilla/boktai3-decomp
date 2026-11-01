// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
#include "runtime.h"
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
        u32 offset;
        u32 *field;
        u32 value;
        mask = 0x40;
        offset = 0x2d4;
        field = (void *)((u8 *)actor + offset);
        value = *(u16 *)field;
        value &= mask;
        if (value)
            goto one;
        mask = 0x20000;
        offset -= 4;
        field = (void *)((u8 *)actor + offset);
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
    u32 offset = 0x2d0;
    u32 *field;
    u32 value;
    field = (void *)((u8 *)actor + offset);
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
                u32 offset = 0x2cc;
                u32 *field;
                u32 value;
                field = (void *)((u8 *)actor + offset);
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
                        u8 *drawable = (u8 *)actor->drawable + 0x20;
                        u32 value = *(u32 *)(drawable + 8);
                        value |= mask;
                        *(u32 *)(drawable + 8) = value;
                    }
                }

                {
                    register u32 code asm("r1") = 16;
                    u32 offset = 0x2b2;
                    u32 substateOffset;
                    u8 *field;
                    field = (u8 *)actor + offset;
                    {
                        u32 entry = one;
                        *field = entry;
                    }
                    offset -= 2;
                    field = (u8 *)actor + offset;
                    *field = zero;
                    substateOffset = 0x2b5;
                    field = (u8 *)actor + substateOffset;
                    *field = code;
                    {
                        u32 controlOffset = 0x21c;
                        field = (u8 *)actor + controlOffset;
                        {
                            const void *control = defaultControl;
                            *(const void **)field = control;
                        }
                    }
                    substateOffset += 0x13;
                    field = (u8 *)actor + substateOffset;
                    *(u16 *)field = zero;
                    {
                        u32 *flags114 = &actor->flags114;
                        u32 mask = ~1;
                        u32 value;
                        value = *flags114;
                        value &= mask;
                        *flags114 = value;
                    }
                    {
                        u32 fieldOffset = 0x104;
                        field = (u8 *)actor + fieldOffset;
                        *(u32 *)field = zero;
                    }
                    {
                        register u32 state asm("r1") = 3;
                        offset = 0x2a8;
                        field = (u8 *)actor + offset;
                        *field = state;
                    }
                    substateOffset -= 0x1f;
                    field = (u8 *)actor + substateOffset;
                    *field = zero;
                    {
                        register u32 behaviorOffset asm("r1") = 0x2aa;
                        asm("" : "+r"(behaviorOffset));
                        field = (u8 *)actor + behaviorOffset;
                        *field = zero;
                    }
                    offset += 4;
                    field = (u8 *)actor + offset;
                    *(u32 *)field = zero;
                    substateOffset += 8;
                    field = (u8 *)actor + substateOffset;
                    {
                        register u32 entry asm("r1") = one;
                        *field = entry;
                    }
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
                        u32 offset;
                        u32 *field;
                        u32 value;
                        specialCopy = 0x400000;
                        offset = 0x2d0;
                        field = (void *)((u8 *)actor + offset);
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
