#include "scene_actor_contact_list.h"
static inline u8 Unavailable(struct ChaseActor *actor, u16 **savedState, u32 disabled, u32 excluded)
{
    u16 *state = &actor->base.motionFlags;
    u32 flags = *state;
    *savedState = state;
    if ((flags & disabled) || (actor->base.statusFlags & excluded) || actor->base.health <= 0)
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
        disabled = 0x40;
        excluded = 0x20000;
        required = 0x1000;
        invalidMask = 3;
        do {
            if ((node->flags & 1) && (node->flags & 2)) {
                struct ChaseActor *actor = node->owner;
                u16 *state;
                if (!actor)
                    goto invalid;
                if (Unavailable(actor, &state, disabled, excluded) ||
                    (actor->base.behaviorFlags & invalidMask)) {
                invalid:
                    node->flags &= 0xFD;
                    node->owner = 0;
                    node->cooldown = 120;
                    continue;
                }
                {
                    u32 value = *state & required;
                    if (!(u16)value) {
                        node->flags &= 0xFD;
                        node->owner = 0;
                        value = 120;
                        node->cooldown = value;
                    }
                }
            }
            node = node->next;
        } while (node);
    }
}
