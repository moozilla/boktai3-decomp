#include "scene_actor_contact_list.h"
static inline u8 Unavailable(struct SceneActorRegistryActor *inputActor, u16 **savedState,
                             u32 inputDisabled, u32 inputExcluded)
{
    register struct SceneActorRegistryActor *actor = inputActor;
    u32 disabled = inputDisabled;
    u16 *state = &actor->flags2D4;
    u32 excluded = inputExcluded;
    u32 flags;
    flags = *state;
    *savedState = state;
    if ((flags & disabled) || (actor->flags2D0 & excluded) || actor->health <= 0)
        return 1;
    return 0;
}
static inline u32 InvalidFlags(struct SceneActorRegistryActor *actor)
{
    u32 *field = &actor->flags2CC;
    u32 value = *field;
    return value;
}
void SceneActorContacts_ReleaseInvalidOwners(void)
{
    struct SceneActorContactManager *manager = gSceneActorContactManager;
    struct SceneActorContactNode *node;
    if (!manager || Mod(manager->frame, 30))
        return;
    node = manager->head;
    if (!node)
        return;
    {
        u32 disabled = 0x40;
        u32 excluded = 0x20000;
        u32 required = 0x1000;
        u32 invalidMask = 3;
        u32 clearMask = 0xFD;
        do {
            u32 incoming = node->flags;
            u32 flags;
            register struct SceneActorRegistryActor *actor;
            u16 *state;
            {
                u32 active = incoming & 1;
                flags = incoming;
                if (!active)
                    goto advance;
            }
            if (!(flags & 2))
                goto advance;
            actor = node->owner;
            if (!actor)
                goto invalid;
            if (Unavailable(actor, &state, disabled, excluded) ||
                (InvalidFlags(actor) & invalidMask)) {
            invalid: {
                u32 clear = clearMask;
                register u32 zero;
                clear &= flags;
                zero = 0;
                node->flags = clear;
                node->owner = (void *)zero;
                {
                    clear = 120;
                    node->cooldown = clear;
                }
                continue;
            }
            }
            {
                register u32 value = *state;
                u32 mask = required;
                register u32 valid;
                value &= mask;
                valid = (u16)value;
                if (!valid) {
                    u32 clear = clearMask;
                    clear &= flags;
                    node->flags = clear;
                    node->owner = (void *)valid;
                    {
                        clear = 120;
                        node->cooldown = clear;
                    }
                }
            }
        advance:
            node = node->next;
        } while (node);
    }
}
