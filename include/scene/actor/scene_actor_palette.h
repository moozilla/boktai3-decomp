#ifndef GUARD_SCENE_ACTOR_PALETTE_H
#define GUARD_SCENE_ACTOR_PALETTE_H
#include "scene/actor/scene_actor_registry.h"
struct SceneActorPaletteSequenceView {
    u8 unknown0[0x32];
    u16 paletteOffset;
    u8 unknown34[4];
    const u16 *palette;
};
struct SceneActorPaletteTileView {
    u8 unknown0[0x5A];
    u16 paletteOffset;
    u8 unknown5C[0xC];
    const u16 *palette;
};
struct ChaseActor *SceneActorRegistry_FindLiving(u32);
s32 SceneActorRegistry_CountLivingByCategory(s32);
u32 SceneActorRegistry_TestCategoryFlags(s32, u32);
void SceneActorPalette_BeginTransition(u32, u32, u32, s32, u32);
u32 SceneActorPalette_ApplyTransition(struct SceneActorRegistryManager *, struct ChaseActor *);
u32 SceneActorPalette_IsTransitionComplete(void);
void SceneActorPalette_ClearCompletedTransitions(void);
void SceneActorPalette_BindActiveTransition(struct ChaseActor *);
#endif
