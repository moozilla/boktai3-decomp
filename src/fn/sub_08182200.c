#include "global.h"
struct ArmorPaletteController {
    void (*update)(struct ArmorPaletteController *);
    void (*completion)(struct ArmorPaletteController *);
    u8 unknown8[0x488 - 8];
    u16 restoreTimer;
};

struct ArmorPaletteOwner {
    u8 unknown0[0x650];
    struct ArmorPaletteController palette;
    u8 unknownADC[0xB38 - 0xADC];
    u8 kind;
};

void Palette_UpdatePulseFade(struct ArmorPaletteController *);

void Armor_StartAccentPaletteFade(struct ArmorPaletteOwner *owner)
{
    struct ArmorPaletteController *palette = &owner->palette;
    switch (owner->kind) {
        case 3:
        case 5:
        case 7:
        case 9:
        case 0xd:
        case 0xf:
            if (palette->update == 0) {
                palette->update = Palette_UpdatePulseFade;
                palette->completion = 0;
                palette->restoreTimer = 0;
            }
            break;
        default:
            break;
    }
}
