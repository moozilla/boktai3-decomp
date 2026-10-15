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
    u8 unknownAC[0x58];
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
    u8 unknown230[0x28];
    s32 (*update)(void *);
    s32 (*destroy)(void *);
    u8 unknown260[0x48];
    u8 state2A8, phase2A9, behavior2AA, unknown2AB;
    u32 counter2AC;
    u8 field2B0, field2B1, field2B2, field2B3;
    u8 enabled, unknown2B5[3];
    u16 field2B8;
    u8 unknown2BA[6];
    u16 flags2C0;
    u16 field2C2;
    u8 unknown2C4[4];
    u16 field2C8;
    u8 unknown2CA[2];
    u32 flags2CC, flags2D0;
    u16 flags2D4, flags2D6;
    u8 unknown2D8[0x22];
    u16 regionId;
    u8 unknown2FC[0xD4];
    void *context;
    u8 unknown3D4[0x40];
    u16 flags414, field416, field418;
    u8 unknown41A[0x36];
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
    u8 unknown2C[0x14];
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
