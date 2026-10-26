// MATCHING_CONCESSION: register-pinning
#include "scene_actor_palette.h"
/* SceneActorPalette_BindActiveTransition: Binds the active category transition colors and actor palette offset to the actor drawable without advancing the transition */
#define SceneActorPalette_BindActiveTransition sub_08072204
#define SceneActorPalette_BindActiveTransition sub_08072204
void SceneActorPalette_BindActiveTransition(struct SceneActorRegistryActor *actor)
{
    struct SceneActorPaletteManagerView *manager = (void *)gSceneActorRegistryManager;
    register struct SceneActorPaletteTransition *transition asm("r3") = (void *)manager;
    if (!manager)
        return;
    if (actor->category == 10)
        goto category10;
    if (actor->category != 23)
        return;
    transition = &manager->category23;
    goto selected;
category10:
    transition = (void *)((u32)transition + 0x64);
selected:
    if (!transition->remaining)
        return;
    if (actor->drawableKind == 0) {
        ((struct SceneActorPaletteSequenceView *)actor->drawable)->paletteOffset =
            actor->paletteOffset;
        ((struct SceneActorPaletteSequenceView *)actor->drawable)->palette = transition->colors;
    } else {
        struct TilePaletteTail {
            u8 unknown0[0x3A];
            u16 paletteOffset;
        };
        struct TilePaletteTail *tail = (void *)((u32)actor->drawable + 0x20);
        tail->paletteOffset = actor->paletteOffset;
        ((struct SceneActorPaletteTileView *)actor->drawable)->palette = transition->colors;
    }
}
