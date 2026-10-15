#ifndef GUARD_SCENE_ACTOR_PALETTE_H
#define GUARD_SCENE_ACTOR_PALETTE_H
#include "scene_actor_registry.h"
/* Registry categories 10 and 23 have independent 32-step RGB15 transitions. */
struct SceneActorPaletteTransition {
    u16 colors[16];
    u16 remaining, destinationBank;
    s16 red[16], green[16], blue[16];
    s8 deltaRed[16], deltaGreen[16], deltaBlue[16];
};
struct SceneActorPaletteManagerView {
    u8 unknown0[0x2C];
    u32 flags;
    u8 unknown30[0x34];
    struct SceneActorPaletteTransition category10, category23;
};
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
struct SceneActorRegistryActor *SceneActorRegistry_FindLiving(u32);
s32 SceneActorRegistry_CountLivingByCategory(s32);
u32 SceneActorRegistry_TestCategoryFlags(s32, u32);
void sub_08071E00(u32, u32, u32, s32, u32);
u32 sub_08071F50(struct SceneActorPaletteManagerView *, struct SceneActorRegistryActor *);
u32 sub_08072124(void);
void SceneActorPalette_ClearCompletedTransitions(void);
void sub_08072204(struct SceneActorRegistryActor *);
#endif
