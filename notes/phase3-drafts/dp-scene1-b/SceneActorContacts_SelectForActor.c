#include "scene_actor_state.h"
#include "scene_actor_contacts.h"
static inline u8 SceneActorContacts_AreDisabled(void)
{
    u32 mask = 0x40;
    if (gGameState->flags868 & mask)
        return 1;
    return 0;
}
struct SceneActorContactNode *
SceneActorContacts_SelectForActor(struct SceneActorRegistryActor *argument, u32 kindMaskArgument)
{
    struct SceneActorRegistryActor *actor = argument;
    u32 kindMask = kindMaskArgument;
    register struct SceneActorRegistryActor *owner = actor;
    struct SceneActorContactManager *manager;

    struct SceneActorContactNode *node, *best;
    s32 distance;
    s32 selectionStatus;
    struct SceneActorChaseTracking *contactFlags;
    manager = gSceneActorContactManager;
    if (SceneActorContacts_AreDisabled() || !manager)
        return 0;
    best = 0;
    distance = 0x7FFFFFFF;
    selectionStatus = -1;
    node = manager->head;
    contactFlags = &((struct SceneActorRegistryResetView *)owner)->tracking;
    while (node) {
        if ((node->flags & 1) && node->object && (node->kind & kindMask) &&
            SceneActorContacts_CanSelectNode(node) && SceneActorContacts_EvaluateCandidate(node, actor, &distance)) {
            best = node;
            selectionStatus = (s32)best;
        }
        node = node->next;
    }
    if (selectionStatus >= 0) {
        switch (best->kind) {
            case 1:
                best->flags |= 2;
                best->owner = actor;
                break;
            case 2:
            case 4: {
                u32 *flags;
                u32 clearMask;
                u32 value;
                flags = &owner->flags2D0;
                clearMask = ~0x1000000;
                value = *flags;
                value &= clearMask;
                *flags = value;
            }
                SceneActorScript_DispatchKindPayload(best->object);
                break;
        }
    }
    {
        u32 clearMask = ~2;
        struct SceneActorChaseTracking *flags;
        flags = contactFlags;
        clearMask &= flags->flags;
        flags->flags = clearMask;
    }
    return best;
}
