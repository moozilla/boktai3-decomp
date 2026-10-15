// MATCHING_CONCESSION: register-pinning
#include "scene_actor_contact_list.h"
#define SceneActorContacts_CanSelectNode sub_08072BC4
u8 SceneActorContacts_CanSelectNode(struct SceneActorContactNode *node)
{
    register u32 flags asm("r1") = node->flags;
    if ((flags & 2) || node->cooldown || (node->kind == 1 && !TetherActor_CanStartInteraction()))
        return 0;
    return 1;
}
