// MATCHING_CONCESSION: register-pinning
#include "scene_actor_palette.h"
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return TRUE;
    return FALSE;
}
static inline u32 ReadMasked(u32 mask, u32 *field)
{
    return *field & mask;
}
static inline u8 Unavailable(struct SceneActorRegistryActor *actor, u32 disabled, u32 excluded)
{
    if (actor) {
        register u32 mask asm("r1") = disabled;
        u32 offset = 0x2D4;
        u32 value;
        asm("" : "+r"(offset), "+r"(mask));
        value = *(u16 *)((u32)actor + offset);
        value &= mask;
        if (value)
            return 1;
        mask = excluded;
        offset -= 4;
        {
            u32 *field = (void *)((u32)actor + offset);
            value = *field;
        }
        value &= mask;
        if (value || actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline u16 Remaining(struct SceneActorPaletteTransition *transition)
{
    register struct SceneActorPaletteTransition *low asm("r1") = transition;
    return low->remaining;
}
static inline s32 ReadSignedByte(const s8 *input)
{
    u32 encoded = *(const u8 *)input;
    encoded <<= 24;
    return (s32)encoded >> 24;
}
static inline void BindPalette(struct SceneActorRegistryActor *actor,
                               struct SceneActorPaletteTransition *inputTransition)
{
    register struct SceneActorPaletteTransition *transition asm("r12") = inputTransition;
    if (actor->drawableKind == 0) {
        void *drawable = actor->drawable;
        const u16 *colors;
        colors = transition->colors;
        ((struct SceneActorPaletteSequenceView *)drawable)->palette = colors;
    } else {
        void *drawable = actor->drawable;
        const u16 *colors;
        colors = transition->colors;
        ((struct SceneActorPaletteTileView *)drawable)->palette = colors;
    }
}
u32 SceneActorPalette_ApplyTransition(struct SceneActorPaletteManagerView *inputManager,
                                      struct SceneActorRegistryActor *inputActor)
{
    struct SceneActorPaletteManagerView *manager = inputManager;
    register struct SceneActorPaletteTransition *transition asm("r12");
    register struct SceneActorRegistryActor *actorInput asm("r1") = inputActor;
    struct SceneActorRegistryActor *actor;
    u32 alreadyAdvanced;
    u32 category;
    register u32 categoryFlags asm("r1");
    u32 flagSet;
    register s32 i asm("r5");
    struct {
        s8 *deltaRed;
        s16 *red;
        s8 *deltaGreen;
        s16 *green;
        u32 *actorFlags;
        s16 *blue;
    } channels;
    asm("" : "+r"(manager), "+r"(actorInput));
    actor = actorInput;
    if (Unavailable(actor, 0x40, 0x20000))
        goto failure;
    {
        u32 mask = 0x2000;
        struct SceneActorPaletteManagerView *low = manager;
        categoryFlags = low->flags;
        if (!Test(categoryFlags, mask))
            goto failure;
    }
    alreadyAdvanced = 0;
    category = actor->category;
    if (category == 10)
        goto category10;
    if (category != 23)
        goto success;
    transition = &manager->category23;
    if (categoryFlags & 0x8000)
        goto flagPresent;
    goto flagAbsent;
category10:
    transition = &manager->category10;
    if (!(categoryFlags & 0x4000))
        goto flagAbsent;
flagPresent:
    flagSet = 1;
    goto selected;
flagAbsent:
    flagSet = 0;
selected:
    asm("" : "+r"(flagSet));
    if (flagSet)
        alreadyAdvanced = 1;
    if (!Remaining(transition)) {
        if (!ReadMasked(0x02008000, &actor->flags2CC))
            BindPalette(actor, transition);
        goto success;
    }
    {
        u32 offset = 0x2CC;
        asm("" : "+r"(offset));
        channels.actorFlags = (void *)((u32)actor + offset);
    }
    if (!alreadyAdvanced) {
        s8 *deltaBlue;
        register u16 *colorCursor asm("r6");
        i = 0;
        {
            s16 *red = transition->red;
            s8 *deltaRed;
            register s16 *green asm("r0");
            channels.red = red;
            deltaRed = transition->deltaRed;
            channels.deltaRed = deltaRed;
            green = transition->green;
            channels.green = green;
            channels.deltaGreen = (void *)((u32)red + 0x70);
            channels.blue = (void *)((u32)deltaRed - 0x20);
        }
        {
            s8 *base = transition->deltaBlue;
            deltaBlue = base;
        }
        colorCursor = transition->colors;
    nextColor: {
        u32 halfOffset = i * 2;
        s16 *redValue;
        s16 *greenValue;
        register s32 blueSum asm("r2");
        {
            register s16 *base asm("r2") = channels.red;
            redValue = (void *)((u32)base + halfOffset);
        }
        {
            register s8 *base asm("r2") = channels.deltaRed;
            register s8 *input asm("r0");
            input = base + i;
            asm("" : "+r"(input));
            *redValue += ReadSignedByte(input);
        }
        {
            register s16 *base asm("r0") = channels.green;
            greenValue = (void *)((u32)base + halfOffset);
        }
        {
            register s8 *base asm("r2") = channels.deltaGreen;
            register s8 *input asm("r0");
            input = base + i;
            asm("" : "+r"(input));
            *greenValue += ReadSignedByte(input);
        }
        {
            register s16 *base asm("r0") = channels.blue;
            s16 *blueValue;
            register s8 *input asm("r0");
            blueValue = (void *)((u32)base + halfOffset);
            input = deltaBlue + i;
            blueSum = *input;
            blueSum += (u16)*blueValue;
            *blueValue = blueSum;
        }
        {
            s32 packed = *redValue >> 5;
            s32 green = *greenValue >> 5;
            packed |= (u32)green << 5;
            blueSum = (s16)blueSum >> 5;
            packed |= (u32)blueSum << 10;
            *colorCursor++ = packed;
        }
    }
        i++;
        if (i < 16)
            goto nextColor;
        {
            struct SceneActorPaletteTransition *low = transition;
            low->remaining--;
        }
        {
            u32 mask;
            register u32 current asm("r2") = category;
            if (current == 10)
                goto firstCategory;
            if (current != 23)
                goto stepFinished;
            mask = 0x8000;
            goto writeFlag;
        firstCategory:
            mask = 0x4000;
        writeFlag: {
            struct SceneActorPaletteManagerView *low = manager;
            low->flags |= mask;
        }
        }
    }
stepFinished: {
    u32 mask = 0x02008000;
    register u32 *field asm("r2");
    field = channels.actorFlags;
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
