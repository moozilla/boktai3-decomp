#include "scene/actor/scene_actor_palette.h"
/* SceneActorPalette_BindActiveTransition: Binds the active category transition colors and actor palette offset to the actor drawable without advancing the transition */
void SceneActorPalette_BindActiveTransition(struct ChaseActor *actor)
{
    struct SceneActorPaletteManagerView *manager = (void *)gSceneActorRegistryManager;
    struct SceneActorPaletteManagerView *view = manager;
    struct SceneActorPaletteTransition *transition;

    if (!manager)
        return;
    switch (actor->base.category) {
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
    if (actor->base.drawableKind == 0) {
        ((struct SceneActorPaletteSequenceView *)actor->base.drawable)->paletteOffset =
            actor->base.paletteOffset;
        ((struct SceneActorPaletteSequenceView *)actor->base.drawable)->palette =
            transition->colors;
    } else {
        struct TilePaletteTail {
            u8 unknown0[0x3A];
            u16 paletteOffset;
        };
        struct TilePaletteTail *tail = (void *)((u32)actor->base.drawable + 0x20);
        tail->paletteOffset = actor->base.paletteOffset;
        ((struct SceneActorPaletteTileView *)actor->base.drawable)->palette = transition->colors;
    }
}
