#ifndef GUARD_SCENE_ACTOR_REGISTRY_H
#define GUARD_SCENE_ACTOR_REGISTRY_H
#include "engine.h"
#include "contact_actor.h"
/* Tentative actor-registry manager. Narrow views cover only reviewed fields. */
struct SceneActorRegistryActor {
    void *object;
    void *drawable;
    u8 drawableKind, unknown9[0x53];
    struct ContactPosition position;
    u8 unknown64[0x34];
    u8 state, unknown99[3];
    s16 health;
    u8 unknown9E[0x1BA];
    s32 (*update)(void *);
    s32 (*destroy)(void *);
    u8 unknown260[0x6C];
    u32 flags2CC, flags2D0;
    u16 flags2D4, flags2D6;
    u8 unknown2D8[0xF8];
    void *context;
};
struct SceneActorRegistryNode {
    struct SceneActorRegistryNode *next;
    struct SceneActorRegistryActor *actor;
};
struct SceneActorRegistryManager {
    u8 task[0x24];
    u32 resourceId;
    u16 unknown28, frame;
    u32 flags;
    u8 mode, unknown31;
    u16 field32;
    u8 unknown34[0x2E];
    u16 field62;
    u8 unknown64[0x21C];
    u32 subsystem[2];
};
extern struct SceneActorRegistryManager *gUnk_020004A8;
extern struct SceneActorRegistryNode *gUnk_020004AC;
void sub_08070FA8(struct SceneActorRegistryManager *);
void sub_08071434(void);
void sub_08071470(struct SceneActorRegistryManager *);
void sub_080714E4(struct SceneActorRegistryActor *);
void sub_0807171C(struct SceneActorRegistryActor *);
void sub_080718D8(struct SceneActorRegistryActor *);
void sub_08071CB0(struct SceneActorRegistryManager *);
s32 sub_08071CFC(struct SceneActorRegistryManager *);
s32 sub_08071D3C(struct SceneActorRegistryManager *);
s32 sub_08071D60(struct SceneActorRegistryManager *, u32, u32);
struct SceneActorRegistryManager *sub_08071D9C(u32, u32);
void sub_08071DF4(void);
#endif
