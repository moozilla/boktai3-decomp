#include "modifier_list.h"
#include "palette_pulse.h"
#include "render.h"
void PaletteFade_CycleMenuColors(struct CharacterPaletteCycle *);
void CharacterModel_UpdatePairedAnimations(void *, s32);
void ModifierMenu_UpdatePreviewAppearance(struct ModifierMenu *menu)
{
    Palette_UpdateHighlightPulse(&menu->palette[12], &menu->colorCycle);
    if (menu->state != 1) {
        PaletteFade_CycleMenuColors(&menu->paletteCycle);
        CharacterModel_UpdatePairedAnimations((u8 *)menu + 0xE34, 0);
    } else {
        u16 *palette = gBgPaletteBuffer;
        u32 color = 0x73FF;
        u16 *p;
        s32 i;
        p = &palette[17];
        i = 6;
        do {
            *p++ = color;
        } while (--i >= 0);
        {
            u32 finalColor = 0x73FF;
            palette[25] = finalColor;
        }
    }
}
