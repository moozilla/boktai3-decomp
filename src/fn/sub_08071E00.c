// MATCHING_CONCESSION: register-pinning
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
    register struct SceneActorPaletteManagerView *manager asm("r8") = (void *)gUnk_020004A8;
    struct SceneActorPaletteTransition *transition;
    register const u16 *destination asm("r6");
    const u16 *source;
    u8 *palettes;
    register u32 destinationOffset asm("r1");
    u32 sourceOffset;
    register s32 i asm("r12");
    register u16 *colorCursor asm("r10");
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
        register s8 *red asm("r4") = transition->deltaRed;
        register s8 *green asm("r5");
        register s8 *blue asm("r3");
        channels.deltaRed = red;
        green = transition->deltaGreen;
        channels.deltaGreen = green;
        blue = transition->deltaBlue;
        channels.deltaBlue = blue;
        channels.red = (void *)((u32)red - 0x60);
        channels.green = (void *)((u32)green - 0x50);
        channels.blue = (void *)((u32)blue - 0x40);
    }
    destination = (void *)((u32)palettes + destinationOffset);
    {
        register u32 zero asm("r4") = 0;
        asm("" : "+r"(zero));
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
        register struct SceneActorPaletteManagerView *low asm("r5") = manager;
        if (Test(low->flags, 0x2000) && transition->remaining != 32) {
            register u16 *current asm("r2") = colorCursor;
            base = *current;
        } else {
            register const u16 *sample asm("r3") = source;
            asm("" : "+r"(sample));
            base = *sample;
        }
    }
    /* Separate color-mask lifetimes prevent a loop-wide cached mask. */
    {
        register s8 *redOutput asm("r2") = &channels.deltaRed[i];
        u32 toRed;
        u32 redMask;
        u32 baseMask;
        toRed = *(const u8 *)destination;
        redMask = 31;
        redMask &= toRed;
        red = base;
        asm("" : "+r"(red));
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
        register s8 *blueOutput asm("r4") = &channels.deltaBlue[i];
        u32 toBlue;
        register u32 destinationMask asm("r1");
        toBlue = *destination >> 10;
        destinationMask = 31;
        toBlue &= destinationMask;
        blue = base >> 10;
        {
            register u32 baseMask asm("r5") = 31;
            blue &= baseMask;
        }
        *blueOutput = toBlue - blue;
    }
    *(s16 *)((u32)channels.red + halfOffset) = red << 5;
    *(s16 *)((u32)channels.green + halfOffset) = green << 5;
    *(s16 *)((u32)channels.blue + halfOffset) = blue << 5;
    destination++;
    {
        u32 stride = 2;
        halfOffset += stride;
        {
            register const u16 *next asm("r1") = source + 1;
            asm("" : "+r"(next));
            source = next;
        }
        colorCursor = (void *)((u32)colorCursor + stride);
    }
    {
        register s32 step asm("r2") = 1;
        asm("" : "+r"(step));
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
        register struct SceneActorPaletteManagerView *low asm("r4") = manager;
        low->flags = mask | low->flags;
        transition->remaining = 32;
        if (auxiliaryFlag) {
            mask = 0x20000;
            low->flags = mask | low->flags;
        } else {
            mask = ~0x20000;
            {
                register struct SceneActorPaletteManagerView *other asm("r5") = manager;
                other->flags = mask & other->flags;
            }
        }
    }
}
