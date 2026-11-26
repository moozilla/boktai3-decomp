#ifndef GUARD_SCENE_ACTOR_REGISTRY_H
#define GUARD_SCENE_ACTOR_REGISTRY_H
#include "engine/engine.h"
#include "scene/actor/contact_actor.h"
#include "monsters/chase_actor.h"
/* Tentative actor-registry manager. Narrow views cover only reviewed fields. */
/* Identity lookup view begins at actor +0x3D4 (0813760C). */
struct SceneActorTargetIdentityView {
    u8 unknown0[0x64];
    void *identity;
};
/* Embedded control block beginning at actor +0x3D4. */
struct SceneActorRegistryCallbackBlock {
    u8 unknown0[0x70];
    void *fallback;
};
struct SceneActorRegistrySequenceDrawable {
    u32 flags;
    u8 unknown4[0x18];
    struct ContactPosition position;
    u8 unknown24[8], animation[6];
    u16 paletteBank;
};
struct SceneActorRegistryTileDrawable {
    u8 unknown0[0x28];
    u32 flags;
    u16 state2C;
    u8 unknown2E[6];
    u16 parameter34;
    u8 unknown36[0xA];
    struct ContactPosition position;
    u8 unknown48[0x12];
    u16 paletteBank;
    u8 unknown5C[0xC];
    u8 *palette;
};
union SceneActorRegistryPackedPosition {
    struct ContactPosition position;
    struct {
        u32 x : 16, y : 16, z : 16, padding : 16;
    } packed;
};
struct SceneActorRegistryNode {
    struct SceneActorRegistryNode *next;
    struct ChaseActor *actor;
};
struct SceneActorRegistryMessage {
    u16 receiverId, unknown2;
    u8 *data;
};
struct SceneActorRegistryManager {
    struct Task task;
    struct SceneActorRegistryMessage *messages;
    struct SceneActorRegistryNode *sentinel, *cursor;
    u32 resourceId;
    u16 unknown28, frame;
    u32 flags;
    u8 mode, unknown31;
    u16 field32;
    struct Task *controlTask, *variantTask, *attachmentTask;
    u8 unknown40[0x10];
    struct Task *category14Task;
    u16 controlTaskId, variantTaskId, attachmentTaskId;
    u8 unknown5A[6];
    u16 category14TaskId;
    u16 field62;
    u8 unknown64[0x21C];
    u32 subsystem[2];
};
/* 0x020004A8 */
extern struct SceneActorRegistryManager *gSceneActorRegistryManager;
/* 0x020004AC */
extern struct SceneActorRegistryNode *gSceneActorRegistryNode;
void SceneActorRegistry_UpdateActors(struct SceneActorRegistryManager *);
void SceneActorRegistry_DestroyActors(void);
void SceneActorRegistry_AdvanceInactiveActors(struct SceneActorRegistryManager *);
void SceneActorRegistry_ResetRouteAndColliders(struct ChaseActor *);
void SceneActorRegistry_ResetRoute(struct ChaseActor *);
void SceneActorRegistry_RestoreActorDefaults(struct ChaseActor *);
void SceneActorRegistry_UpdateBackgroundFlag(struct SceneActorRegistryManager *);
s32 SceneActorRegistry_TickManager(struct SceneActorRegistryManager *);
s32 SceneActorRegistry_DestroyManager(struct SceneActorRegistryManager *);
s32 SceneActorRegistry_InitializeManager(struct SceneActorRegistryManager *, u32, u32);
struct SceneActorRegistryManager *SceneActorRegistry_CreateManager(u32, u32);
void SceneActorRegistry_ResetManager(void);
#endif
