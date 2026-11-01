#ifndef GUARD_SCENE_ACTOR_REGISTRY_H
#define GUARD_SCENE_ACTOR_REGISTRY_H
#include "engine.h"
#include "contact_actor.h"
/* Tentative actor-registry manager. Narrow views cover only reviewed fields. */
struct SceneActorRegistryActor {
    void *object;
    void *drawable;
    u8 drawableKind, unknown9[0x47];
    u32 id;
    u8 unknown54[8];
    struct ContactPosition position;
    struct ContactPosition velocity;
    u8 unknown6C[0x2C];
    u8 category, unknown99[3];
    s16 health;
    u8 unknown9E[0xA];
    s16 fieldA8;
    u16 fieldAA;
    u8 unknownAC[0xC];
    u16 effectDurationB8; /* Tentative: particle lifetime argument, 080DA810. */
    u8 unknownBA[6];
    u16 effectParameterC0; /* Particle constructor fourth argument. */
    u8 unknownC2[0x1A];
    struct SceneActorPositionSource *distanceTarget; /* +0xDC, 0813760C. */
    u8 unknownE0[0x24];
    u32 field104;
    u8 unknown108[0xC];
    u32 flags114;
    u8 unknown118[0x60];
    u16 field178;
    u8 unknown17A[0x64];
    u16 paletteBank, basePaletteBank;
    u8 unknown1E2[4];
    u16 paletteOffset;
    u8 unknown1E8[0x34];
    const void *control;
    u8 unknown220[0xC];
    const void *defaultControl;
    u8 unknown230[0x1C];
    void *messageCallback;
    u8 unknown250[8];
    s32 (*update)(void *);
    s32 (*destroy)(void *);
    u8 unknown260[4];
    void *activeCallback264; /* 0812F12C: dispatched with actor. */
    u8 unknown268[0xC];
    void *contextCallback274; /* 0812F12C: dispatched with context. */
    u8 unknown278[0x30];
    u8 state2A8, phase2A9, behavior2AA, unknown2AB;
    u32 counter2AC;
    u8 field2B0, field2B1, field2B2, field2B3;
    u8 enabled, unknown2B5[3];
    u16 field2B8;
    u8 unknown2BA[6];
    u16 flags2C0;
    u16 field2C2;
    s32 livingCountByCategory;
    u16 field2C8;
    u8 unknown2CA[2];
    u32 flags2CC, flags2D0;
    u16 flags2D4, flags2D6;
    u8 unknown2D8[0x22];
    u16 regionId;
    u8 unknown2FC[0xC];
    s32 profileIndex;
    u8 unknown30C[0xC4];
    void *context;
    u8 unknown3D4[0x40];
    u16 flags414, field416, field418;
    u8 unknown41A[0x1A];
    u16 distanceTargetLock; /* +0x434, 0813760C. */
    u8 unknown436[2];
    void *distanceTargetIdentity; /* +0x438, 0813760C. */
    u8 unknown43C[8];
    void *fallbackCallback444;
    u8 unknown448[8];
    const struct ContactPosition *field450;
    u8 unknown454[0x28];
    u32 effectConfig[2];
    const struct ContactPosition *effectOrigin;
    u8 unknown488[0xE8];
    void *field570, *field574;
    u8 unknown578[0x64];
    void *field5DC;
    u8 unknown5E0[0x14];
    const void *control5F4;
};
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
    struct SceneActorRegistryActor *actor;
};
struct SceneActorRegistryMessage {
    u16 receiverId, unknown2;
    u8 *data;
};
struct SceneActorRegistryManager {
    u8 task[0x18];
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
void SceneActorRegistry_ResetRouteAndColliders(struct SceneActorRegistryActor *);
void SceneActorRegistry_ResetRoute(struct SceneActorRegistryActor *);
void SceneActorRegistry_RestoreActorDefaults(struct SceneActorRegistryActor *);
void SceneActorRegistry_UpdateBackgroundFlag(struct SceneActorRegistryManager *);
s32 SceneActorRegistry_TickManager(struct SceneActorRegistryManager *);
s32 SceneActorRegistry_DestroyManager(struct SceneActorRegistryManager *);
s32 SceneActorRegistry_InitializeManager(struct SceneActorRegistryManager *, u32, u32);
struct SceneActorRegistryManager *SceneActorRegistry_CreateManager(u32, u32);
void SceneActorRegistry_ResetManager(void);
#endif
