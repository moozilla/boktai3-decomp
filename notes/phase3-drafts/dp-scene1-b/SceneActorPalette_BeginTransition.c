#include "scene_actor_palette.h"
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return TRUE;
    return FALSE;
}
void SceneActorPalette_BeginTransition(u32 destinationBank, u32 sourceBank, u32 unused,
                                       s32 category, u32 auxiliaryFlag)
{
    register struct SceneActorPaletteManagerView *manager =
        (void *)gSceneActorRegistryManager;
    struct SceneActorPaletteTransition *transition;
    register const u16 *destination;
    const u16 *source;
    u8 *palettes;
    register u32 destinationOffset;
    u32 sourceOffset;
    register s32 i;
    register u16 *colorCursor;
    u32 halfOffset;
    struct {
        s8 *deltaBlue;
        s16 *green;
        s8 *deltaRed;
        s8 *deltaGreen;
        s16 *red;
        s16 *blue;
    } channels;
    if (!manager)
        return;
    if (category == 10)
        goto category10;
    if (category != 23)
        return;
    transition = &manager->category23;
    goto selected;
category10:
    transition = &manager->category10;
selected:
    SceneActorRegistry_FindLiving(category);
    {
        u8 **bankPointer = &gPaletteBankData;
        destinationOffset = destinationBank * 32;
        palettes = *bankPointer;
        sourceOffset = sourceBank * 32;
    }
    transition->destinationBank = destinationBank;
    i = 0;
    {
        register s8 *red = transition->deltaRed;
        register s8 *green;
        register s8 *blue;
        channels.deltaRed = red;
        green = transition->deltaGreen;
        channels.deltaGreen = green;
        blue = transition->deltaBlue;
        channels.deltaBlue = blue;
        channels.red = transition->red;
        channels.green = transition->green;
        channels.blue = transition->blue;
    }
    destination = (void *)((u32)palettes + destinationOffset);
    {
        register u32 zero = 0;
        halfOffset = zero;
    }
    source = (void *)((u32)palettes + sourceOffset);
    colorCursor = transition->colors;
nextColor: {
    s32 base;
    s32 red;
    s32 green;
    s32 blue;
    {
        register struct SceneActorPaletteManagerView *low = manager;
        if (Test(low->flags, 0x2000) && transition->remaining != 32) {
            register u16 *current = colorCursor;
            base = *current;
        } else {
            register const u16 *sample = source;
            base = *sample;
        }
    }
    /* Separate color-mask lifetimes prevent a loop-wide cached mask. */
    {
        register s8 *redOutput = &channels.deltaRed[i];
        u32 toRed;
        u32 redMask;
        u32 baseMask;
        toRed = *(const u8 *)destination;
        redMask = 31;
        redMask &= toRed;
        red = base;
        baseMask = 31;
        red &= baseMask;
        *redOutput = redMask - red;
        {
            s8 *greenOutput = &channels.deltaGreen[i];
            u32 toGreen;
            toGreen = (*destination >> 5) & baseMask;
            green = (base >> 5) & baseMask;
            *greenOutput = toGreen - green;
        }
    }
    {
        register s8 *blueOutput = &channels.deltaBlue[i];
        u32 toBlue;
        register u32 destinationMask;
        toBlue = *destination >> 10;
        destinationMask = 31;
        toBlue &= destinationMask;
        blue = base >> 10;
        {
            register u32 baseMask = 31;
            blue &= baseMask;
        }
        *blueOutput = toBlue - blue;
    }
    channels.red[i] = red << 5;
    channels.green[i] = green << 5;
    channels.blue[i] = blue << 5;
    destination++;
    {
        u32 stride = 2;
        halfOffset += stride;
        {
            register const u16 *next = source + 1;
            source = next;
        }
        colorCursor = (void *)((u32)colorCursor + stride);
    }
    {
        register s32 step = 1;
        i += step;
    }
    {
        s32 test = i;
        if (test <= 15)
            goto nextColor;
    }
}
    {
        u32 mask = 0x2000;
        register struct SceneActorPaletteManagerView *low = manager;
        low->flags = mask | low->flags;
        transition->remaining = 32;
        if (auxiliaryFlag) {
            mask = 0x20000;
            low->flags = mask | low->flags;
        } else {
            mask = ~0x20000;
            {
                register struct SceneActorPaletteManagerView *other = manager;
                other->flags = mask & other->flags;
            }
        }
    }
}
