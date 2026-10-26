#include "global.h"
#include "engine.h"
void Palette_InterpolateBanks(void *, void *, void *, int, int);

typedef struct {
    u8 unk0[0x10];
    s16 mode;
    s16 counter;
    u32 bankB;
    u32 bankA;
} SceneCharacterPaletteFade;

s32 SceneCharacter_UpdatePaletteFade(u8 *a, u8 *b)
{
    s16 zero = 0;
    SceneCharacterPaletteFade *fade = (SceneCharacterPaletteFade *)(b + 0x20);
    s32 counter = fade->counter;

    if (fade->mode == 1) {
        u32 idx = fade->bankA;
        u8 *from = gPaletteBankData + (idx << 5);
        u8 *to = gPaletteBankData + (fade->bankB << 5);
        Palette_InterpolateBanks(b + 0x1cc, from, to, counter, 6);
        if (counter > 62) {
            fade->mode = zero;
        }
    } else if (fade->mode == 2) {
        u32 idx = fade->bankB;
        u8 *from = gPaletteBankData + (idx << 5);
        u8 *to = gPaletteBankData + (fade->bankA << 5);
        Palette_InterpolateBanks(b + 0x1cc, from, to, counter, 6);
        if (counter > 62) {
            fade->mode = zero;
            fade->bankB = fade->bankA;
        }
    } else if (fade->mode == 3) {
        if (counter > 126) {
            counter = 0;
            fade->counter = 0;
        }
        if (counter <= 63) {
            u32 idx = fade->bankB;
            u8 *from = gPaletteBankData + (idx << 5);
            u8 *to = gPaletteBankData + (fade->bankA << 5);
            Palette_InterpolateBanks(b + 0x1cc, from, to, counter, 6);
        } else {
            u32 idx = fade->bankA;
            u8 *from = gPaletteBankData + (idx << 5);
            u8 *to = gPaletteBankData + (fade->bankB << 5);
            Palette_InterpolateBanks(b + 0x1cc, from, to, counter - 64, 6);
        }
    }

    fade->counter++;
}
