// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
s32 TetherActor_SetEntry(struct TetherActor *, u32);
s32 DisplayAnimation_InitializeKeyframes(struct DisplayAnimation *, u32, s32,
                                         const struct DisplayPosition *);
s32 DisplayAnimation_InitializeSequence(struct DisplayAnimation *, s32, u32, s32,
                                        const struct DisplayPosition *);
void DisplayAnimation_SelectTiles(struct DisplayAnimation *, u32);
void DisplayAnimation_SetTileSource(struct DisplayAnimation *, u32);
void DisplayAnimation_SetPriority(struct DisplayAnimation *, u32);
void TetherActor_InitDisplayFromScript(struct TetherActor *actor)
{
    struct {
        u32 zero;
        struct DisplayPosition position;
    } packet;
    struct DisplayAnimation *display;
    u32 zero;
    struct DisplayPosition *initialPosition;
    u16 *paletteIndex = &actor->paletteIndex;
    u32 value;
    zero = 0;
    *paletteIndex = 10;
    initialPosition = &packet.position;
    packet.zero = zero;
    {
        register u32 *fill asm("r0") = &packet.zero;
        register struct DisplayPosition *destination asm("r1");
        destination = initialPosition;
        CpuSet(fill, destination, 0x5000002);
    }
    if (actor->kind == 9) {
        display = &actor->display.animation;
        DisplayAnimation_InitializeKeyframes(display, 0x35E3, 0, initialPosition);
    } else {
        register const u16 *palette asm("r0");
        u16 *buffer;
        const u16 *source;
        u16 *destination;
        s32 i;
        display = &actor->display.animation;
        asm("" : "+r"(display));
        DisplayAnimation_InitializeSequence(display, 0x2499, 0x3D95, 0, initialPosition);
        TetherActor_SetEntry(actor, 0);
        actor->appliedPalette = *paletteIndex;
        DisplayAnimation_SelectTiles(display, actor->appliedPalette + 0x283);
        if (display->kind == 2)
            palette = actor->display.colors.keyframePalette;
        else
            palette = actor->display.colors.palette;
        buffer = actor->palette;
        source = palette;
        destination = buffer;
        i = 15;
        do {
            *destination++ = *source++;
        } while (--i >= 0);
        DisplayAnimation_SetTileSource(display, (u32)buffer);
    }
    DisplayAnimation_SetPriority(display, 2);
    value = Script_SeekToKeyword('d');
    if (value)
        actor->heading = Script_GetValue();
    else
        actor->heading = value;
    actor->shakeX = 0;
    actor->displayOffsetY = 0;
    actor->shakeZ = 0;
}
