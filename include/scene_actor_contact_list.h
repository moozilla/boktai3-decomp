#ifndef GUARD_SCENE_ACTOR_CONTACT_LIST_H
#define GUARD_SCENE_ACTOR_CONTACT_LIST_H
#include "scene_actor_contacts.h"
u8 sub_08072BF0(struct SceneActorContactNode *, void *, u32);
void sub_08072C40(struct SceneActorContactNode *);
struct SceneActorContactNode *sub_08072C80(u32);
u8 sub_08072CB0(struct SceneActorContactNode *);
void *sub_08072CF0(u16);
u32 TetherActor_CanStartInteraction(void);
struct SceneActorContactGateView {
    u8 unknown0[0xF];
    u8 flags;
    u8 unknown10[0x14];
    u32 enabled;
};
void sub_08072E50(struct SceneActorRegistryActor *, struct SceneActorContactGateView *, u32, u32,
                  u32, u32);
void sub_08072E08(struct SceneActorContactNode *);
#endif
