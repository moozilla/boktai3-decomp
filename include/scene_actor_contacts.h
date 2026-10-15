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
void sub_08072B50(void *);
void sub_08072BA8(void);
u8 sub_08072BC4(struct SceneActorContactNode *);
void sub_08072D34(void);
u8 sub_08072F4C(struct SceneActorContactNode *, struct SceneActorRegistryActor *, s32 *);
struct SceneActorContactNode *sub_08073A3C(struct SceneActorRegistryActor *, u32);
void sub_08073B38(void);
u32 sub_08073B60(struct SceneActorContactManager *);
struct SceneActorContactManager *sub_08073B78(struct SceneActorContactManager *);
void sub_08073B84(struct SceneActorContactManager *, u32);
void sub_08073BF0(void);
#endif
