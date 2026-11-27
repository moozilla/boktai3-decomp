#include "npc/story_character/scene_character.h"
#include "engine/engine.h"
void Palette_InterpolateBanks(void *, void *, void *, int, int);

s32 SceneCharacter_UpdatePaletteFade(void *context, struct SceneNpc *actor)
{
    s16 zero = 0;
    struct SceneCharacterEffects *fade = &actor->effects;
    s32 counter = fade->fadeCounter;

    if (fade->fadeMode == 1) {
        u32 idx = fade->bankA;
        u8 *from = gPaletteBankData + (idx << 5);
        u8 *to = gPaletteBankData + (fade->bankB << 5);
        Palette_InterpolateBanks(actor->workPalette, from, to, counter, 6);
        if (counter > 62) {
            fade->fadeMode = zero;
        }
    } else if (fade->fadeMode == 2) {
        u32 idx = fade->bankB;
        u8 *from = gPaletteBankData + (idx << 5);
        u8 *to = gPaletteBankData + (fade->bankA << 5);
        Palette_InterpolateBanks(actor->workPalette, from, to, counter, 6);
        if (counter > 62) {
            fade->fadeMode = zero;
            fade->bankB = fade->bankA;
        }
    } else if (fade->fadeMode == 3) {
        if (counter > 126) {
            counter = 0;
            fade->fadeCounter = 0;
        }
        if (counter <= 63) {
            u32 idx = fade->bankB;
            u8 *from = gPaletteBankData + (idx << 5);
            u8 *to = gPaletteBankData + (fade->bankA << 5);
            Palette_InterpolateBanks(actor->workPalette, from, to, counter, 6);
        } else {
            u32 idx = fade->bankA;
            u8 *from = gPaletteBankData + (idx << 5);
            u8 *to = gPaletteBankData + (fade->bankB << 5);
            Palette_InterpolateBanks(actor->workPalette, from, to, counter - 64, 6);
        }
    }

    fade->fadeCounter++;
}
