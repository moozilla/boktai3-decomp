#include "scene_actor_contact_list.h"
static inline u8 Unavailable(struct SceneActorRegistryActor *actor, u16 *state, u32 disabled,
                             u32 excluded)
{
    if ((*state & disabled) || (actor->flags2D0 & excluded) || actor->health <= 0)
        return 1;
    return 0;
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
        u32 disabled;
        u32 required;
        u32 excluded;
        u32 invalidMask;
        u32 clearMask;
        u32 cooldown;
        disabled = 0x40;
        excluded = 0x20000;
        required = 0x1000;
        invalidMask = 3;
        clearMask = 0xFD;
        cooldown = 120;
        do {
            if ((node->flags & 1) && (node->flags & 2)) {
                struct SceneActorRegistryActor *actor = node->owner;
                u16 *state;
                if (!actor)
                    goto invalid;
                state = &actor->flags2D4;
                if (Unavailable(actor, state, disabled, excluded) || (actor->flags2CC & invalidMask)) {
                invalid:
                    node->flags &= clearMask;
                    node->owner = 0;
                    node->cooldown = 120;
                    continue;
                }
                {
                    u32 value = *state & required;
                    if (!(u16)value) {
                        u16 clear = node->flags & clearMask;
                        node->flags = clear;
                        node->owner = 0;
                        cooldown = 120;
                        node->cooldown = cooldown;
                    }
                }
            }
            node = node->next;
        } while (node);
    }
}
