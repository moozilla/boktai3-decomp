#ifndef GUARD_SCENE_ACTOR_CONTACTS_H
#define GUARD_SCENE_ACTOR_CONTACTS_H
#include "scene_actor_registry.h"
/* Tentative contact-candidate list shared with the preceding 08072264 cohort. */
struct SceneActorContactNode {
    void *object;
    u8 flags, kind;
    u16 cooldown;
    struct SceneActorRegistryActor *owner;
    struct SceneActorContactNode *next;
};
struct SceneActorContactManager {
    u8 task[0x18];
    u32 field18;
    s32 count;
    u32 frame;
    u16 resourceId, unknown26;
    struct SceneActorContactNode *head;
};
extern struct SceneActorContactManager *gUnk_020001A0;
void SceneActorScript_DispatchKindPayload(void *);
void SceneActorContacts_ResetList(void);
u8 SceneActorContacts_CanSelectNode(struct SceneActorContactNode *);
void SceneActorContacts_ReleaseInvalidOwners(void);
u8 sub_08072F4C(struct SceneActorContactNode *, struct SceneActorRegistryActor *, s32 *);
struct SceneActorContactNode *SceneActorContacts_SelectForActor(struct SceneActorRegistryActor *,
                                                                u32);
void SceneActorContacts_TickCooldowns(void);
u32 SceneActorContacts_Update(struct SceneActorContactManager *);
struct SceneActorContactManager *SceneActorContacts_Destroy(struct SceneActorContactManager *);
void sub_08073B84(struct SceneActorContactManager *, u32);
void SceneActorContacts_Reset(void);
#endif
