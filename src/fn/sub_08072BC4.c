#include "scene_actor_contact_list.h"
u8 SceneActorContacts_CanSelectNode(struct SceneActorContactNode *node)
{
    u32 flags = node->flags;
    int mask = 2;
    mask = flags & mask;
    if (mask || node->cooldown || (node->kind == 1 && !TetherActor_CanStartInteraction()))
        return 0;
    return 1;
}
