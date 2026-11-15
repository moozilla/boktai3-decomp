#include "scene_actor_palette.h"
/* SceneActorPalette_BindActiveTransition: Binds the active category transition colors and actor palette offset to the actor drawable without advancing the transition */
#define SceneActorPalette_BindActiveTransition sub_08072204
void SceneActorPalette_BindActiveTransition(struct SceneActorRegistryActor *actor)
{
    struct SceneActorPaletteManagerView *manager = (void *)gSceneActorRegistryManager;
    struct SceneActorPaletteManagerView *view = manager;
    struct SceneActorPaletteTransition *transition;

    if (!manager)
        return;
    switch (actor->category) {
    case 23:
        transition = &manager->category23;
        break;
    case 10:
        transition = &view->category10;
        break;
    default:
        return;
    }
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
