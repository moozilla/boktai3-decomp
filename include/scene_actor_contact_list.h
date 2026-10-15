#ifndef GUARD_SCENE_ACTOR_CONTACT_LIST_H
#define GUARD_SCENE_ACTOR_CONTACT_LIST_H
#include "scene_actor_contacts.h"
u8 SceneActorContacts_RegisterNode(struct SceneActorContactNode *, void *, u32);
void SceneActorContacts_RemoveNode(struct SceneActorContactNode *);
struct SceneActorContactNode *SceneActorContacts_FindNodeByKind(u32);
u8 SceneActorContacts_ContainsActiveNode(struct SceneActorContactNode *);
void *SceneActorContacts_FindObjectById(u16);
u32 TetherActor_CanStartInteraction(void);
struct SceneActorContactGateView {
    u8 unknown0[0xF];
    u8 flags;
    u8 unknown10[0x14];
    u32 enabled;
};
void SceneActorContacts_SetDistanceGate(struct SceneActorRegistryActor *,
                                        struct SceneActorContactGateView *, u32, u32, u32, u32);
void SceneActorContacts_ReleaseNode(struct SceneActorContactNode *);
#endif
