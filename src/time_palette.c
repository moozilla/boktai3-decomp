#include "time_palette.h"

s32 TimePalette_GetSourceIndex(u32 phase)
{
    s32 source;
    switch (phase) {
        case 3:
            source = 1;
            break;
        case 0:
        case 4:
        case 5:
            source = 2;
            break;
        case 1:
        case 2:
        default:
            source = 0;
            break;
    }
    return source;
}

u32 SunClock_GetPhase(void);

void Palette_InterpolateBanks(void *, const void *, const void *, u32, u32);

void TimePalette_UpdateTransition(struct TimePalette *palette)
{
    u32 phase = SunClock_GetPhase();
    if (phase != palette->phase) {
        palette->from = TimePalette_GetSourceIndex(palette->phase);
        palette->to = TimePalette_GetSourceIndex(phase);
        palette->phase = phase;
        palette->timer = 0;
    }
    if (palette->from != palette->to) {
        const u16 *from, *to;
        palette->timer++;
        from = palette->sources[palette->from];
        to = palette->sources[palette->to];
        Palette_InterpolateBanks(palette->colors, from, to, palette->timer, 6);
        if (palette->timer > 0x3f)
            palette->from = palette->to;
    }
}

void Palette_CopyBank(void *, const void *);

void TimePalette_InitTransition(struct TimePalette *palette, const u16 *phase12, const u16 *phase3,
                                const u16 *otherPhases)
{
    u32 zero;
    s32 source;
    source = TimePalette_GetSourceIndex(SunClock_GetPhase());
    zero = 0;
    palette->from = source;
    palette->to = source;
    palette->sources[0] = phase12;
    palette->sources[1] = phase3;
    palette->sources[2] = otherPhases;
    Palette_CopyBank(palette->colors, palette->sources[source]);
    palette->update = TimePalette_UpdateTransition;
    palette->onComplete = (void *)zero;
}
