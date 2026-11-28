#include "scene/collision/scene_actor_contact_list.h"
u8 SceneActorContacts_CanSelectNode(struct SceneActorContactNode *node)
{
    if (node->flags & 2)
        return 0;
    if (node->cooldown)
        return 0;
    if (node->kind == 1 && !TetherActor_CanStartInteraction())
        return 0;
    return 1;
}
