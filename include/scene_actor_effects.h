#ifndef GUARD_SCENE_ACTOR_EFFECTS_H
#define GUARD_SCENE_ACTOR_EFFECTS_H
#include "scene_actor_registry.h"
#include "scene_actor_terrain.h"
/* Packed stack offset: each member is initialized before the effect call. */
struct SceneActorEffectsOffset {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    u32 padding : 16;
};
struct SceneActorEffectsContext {
    u8 kind;
};
struct SceneActorEffectsView {
    void *object, *drawable;
    u8 drawableKind;
    u8 unknown9[0x2B7];
    u16 flags2C0;
    u8 unknown2C2[0xE];
    u32 flags2D0;
    u16 flags2D4;
    u8 unknown2D6[0x4A];
    u8 shadow[0xB4];
    u8 control[0xA8];
    struct SceneActorEffectsContext effect;
};
s32 sub_082315A8(void *, u32, u32, struct SceneActorEffectsOffset *);
#endif
