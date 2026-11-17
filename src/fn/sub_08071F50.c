#include "scene_actor_palette.h"
#include "scene_actor_placement.h"
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return TRUE;
    return FALSE;
}
static inline u8 TestManagerFlag(struct SceneActorPaletteManagerView *manager, u32 mask)
{
    if (manager->flags & mask)
        return TRUE;
    return FALSE;
}
static inline u32 ReadMasked(u32 mask, u32 *field)
{
    return *field & mask;
}
static inline u16 Remaining(struct SceneActorPaletteTransition *transition)
{
    struct SceneActorPaletteTransition *low = transition;
    return low->remaining;
}
static inline void BindPalette(struct ChaseActor *actor,
                               struct SceneActorPaletteTransition *inputTransition)
{
    struct SceneActorPaletteTransition *transition = inputTransition;
    if (actor->base.drawableKind == 0) {
        void *drawable = actor->base.drawable;
        const u16 *colors;
        colors = transition->colors;
        ((struct SceneActorPaletteSequenceView *)drawable)->palette = colors;
    } else {
        void *drawable = actor->base.drawable;
        const u16 *colors;
        colors = transition->colors;
        ((struct SceneActorPaletteTileView *)drawable)->palette = colors;
    }
}
u32 SceneActorPalette_ApplyTransition(struct SceneActorPaletteManagerView *inputManager,
                                      struct ChaseActor *inputActor)
{
    struct SceneActorPaletteManagerView *manager = inputManager;
    struct SceneActorPaletteTransition *transition;
    struct ChaseActor *actorInput = inputActor;
    struct ChaseActor *actor;
    u32 alreadyAdvanced;
    u32 category;
    u32 categoryFlags;
    u32 flagSet;
    s32 i;
    /* Allocation-only: declaration order preserves the six spilled channel homes. */
    s8 *deltaRed;
    s16 *red;
    s8 *deltaGreen;
    s16 *green;
    u32 *actorFlags;
    s16 *blue;
    actor = actorInput;
    if (SceneActorPlacement_IsUnavailable(&actor->base))
        goto failure;
    {
        u32 mask = 0x2000;
        struct SceneActorPaletteManagerView *low = manager;
        categoryFlags = low->flags;
        if (!Test(categoryFlags, mask))
            goto failure;
    }
    alreadyAdvanced = 0;
    category = actor->base.category;
    if (category == 10)
        goto category10;
    if (category != 23)
        goto success;
    transition = &manager->category23;
    flagSet = TestManagerFlag(manager, 0x8000);
    goto selected;
category10:
    transition = &manager->category10;
    flagSet = TestManagerFlag(manager, 0x4000);
selected:
    if (flagSet)
        alreadyAdvanced = 1;
    if (!Remaining(transition)) {
        if (!ReadMasked(0x02008000, &actor->base.behaviorFlags))
            BindPalette(actor, transition);
        goto success;
    }
    actorFlags = &actor->base.behaviorFlags;
    if (!alreadyAdvanced) {
        i = 0;
        red = transition->red;
        deltaRed = transition->deltaRed;
        green = transition->green;
        deltaGreen = transition->deltaGreen;
        blue = transition->blue;
        do {
            s32 blueSum;
            /* Allocation-only: the bounded term folds after loop strength reduction. */
            u32 halfOffset = (i + ((u16)i >> 16)) * 2;
            (*(s16 *)((u32)red + halfOffset)) += deltaRed[i];
            (*(s16 *)((u32)green + halfOffset)) += deltaGreen[i];
            blueSum = (*(s16 *)((u32)blue + halfOffset)) += transition->deltaBlue[i];
            transition->colors[i] = ((*(s16 *)((u32)red + halfOffset)) >> 5) |
                                    ((u32)((*(s16 *)((u32)green + halfOffset)) >> 5) << 5) |
                                    ((u32)((s16)blueSum >> 5) << 10);
            i++;
        } while (i < 16);
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
stepFinished: {
    u32 mask = 0x02008000;
    u32 *field;
    field = actorFlags;
    if (!(*field & mask))
        BindPalette(actor, transition);
}
    if (!Remaining(transition))
        goto success;
failure:
    return 0;
success:
    return 1;
}
