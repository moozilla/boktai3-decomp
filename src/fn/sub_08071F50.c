#include "scene_actor_palette.h"
#include "scene_actor_status.h"

static inline u8 TestManagerFlag(struct SceneActorPaletteManagerView *manager, u32 mask)
{
    if (manager->flags & mask)
        return TRUE;
    return FALSE;
}

static inline void BindPalette(struct RoomActor *actor, const u16 *colors)
{
    if (ActorStatus_BehaviorFlags(actor, 0x02008000))
        return;
    if (actor->drawableKind == 0)
        ((struct SceneActorPaletteSequenceView *)actor->drawable)->palette = colors;
    else
        ((struct SceneActorPaletteTileView *)actor->drawable)->palette = colors;
}

static inline void AdvancePaletteTransition(struct SceneActorPaletteTransition *transition)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        transition->red[i] += transition->deltaRed[i];
        transition->green[i] += transition->deltaGreen[i];
        transition->blue[i] += transition->deltaBlue[i];
        transition->colors[i] = (transition->red[i] >> 5) |
                                ((u32)(transition->green[i] >> 5) << 5) |
                                ((u32)(transition->blue[i] >> 5) << 10);
    }
    transition->remaining--;
}

u32 SceneActorPalette_ApplyTransition(struct SceneActorPaletteManagerView *manager,
                                      struct ChaseActor *actor)
{
    struct SceneActorPaletteTransition *transition;
    u32 alreadyAdvanced;
    u32 category;
    if (ActorStatus_IsDown(actor))
        return 0;
    if (!TestManagerFlag(manager, 0x2000))
        return 0;
    alreadyAdvanced = 0;
    category = actor->base.category;
    switch (category) {
        case 23:
            transition = &manager->category23;
            if (TestManagerFlag(manager, 0x8000))
                alreadyAdvanced = 1;
            break;
        case 10:
            transition = &manager->category10;
            if (TestManagerFlag(manager, 0x4000))
                alreadyAdvanced = 1;
            break;
        default:
            return 1;
    }
    if (!transition->remaining) {
        BindPalette(&actor->base, transition->colors);
        return 1;
    }
    if (!alreadyAdvanced) {
        AdvancePaletteTransition(transition);
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
    BindPalette(&actor->base, transition->colors);
    if (!transition->remaining)
        return 1;
    return 0;
}
