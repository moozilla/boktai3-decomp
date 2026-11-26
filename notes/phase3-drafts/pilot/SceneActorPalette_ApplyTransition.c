#include "scene/actor/scene_actor_palette.h"
#include "scene/actor/scene_actor_placement.h"
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return TRUE;
    return FALSE;
}
static inline void BindPalette(struct SceneActorRegistryActor *actor,
                               struct SceneActorPaletteTransition *transition)
{
    if (actor->drawableKind == 0)
        ((struct SceneActorPaletteSequenceView *)actor->drawable)->palette = transition->colors;
    else
        ((struct SceneActorPaletteTileView *)actor->drawable)->palette = transition->colors;
}
u32 SceneActorPalette_ApplyTransition(struct SceneActorPaletteManagerView *manager,
                                      struct SceneActorRegistryActor *actor)
{
    struct SceneActorPaletteTransition *transition;
    u32 alreadyAdvanced;
    u32 category;
    u32 flagSet;
    s32 i;
    u32 *actorFlags;
    if (SceneActorPlacement_IsUnavailable((struct SceneActorPlacementView *)actor))
        return 0;
    if (!Test(manager->flags, 0x2000))
        return 0;
    alreadyAdvanced = 0;
    category = actor->category;
    switch (category) {
    case 23:
        transition = &manager->category23;
        flagSet = Test(manager->flags, 0x8000);
        break;
    case 10:
        transition = &manager->category10;
        flagSet = Test(manager->flags, 0x4000);
        break;
    default:
        return 1;
    }
    if (flagSet)
        alreadyAdvanced = 1;
    if (transition->remaining == 0) {
        if (!(actor->flags2CC & 0x02008000))
            BindPalette(actor, transition);
        return 1;
    }
    actorFlags = &actor->flags2CC;
    if (!alreadyAdvanced) {
        for (i = 0; i < 16; i++) {
            s32 blue;
            transition->red[i] += transition->deltaRed[i];
            transition->green[i] += transition->deltaGreen[i];
            blue = transition->blue[i] += transition->deltaBlue[i];
            transition->colors[i] = (transition->red[i] >> 5) | ((transition->green[i] >> 5) << 5) |
                                    (((s16)blue >> 5) << 10);
        }
        transition->remaining--;
        switch (category) {
        case 23: {
            u32 mask = 0x8000;
            manager->flags |= mask;
            break;
        }
        case 10: {
            u32 mask = 0x4000;
            manager->flags |= mask;
            break;
        }
        }
    }
    if (!(*actorFlags & 0x02008000))
        BindPalette(actor, transition);
    if (transition->remaining == 0)
        return 1;
    return 0;
}
