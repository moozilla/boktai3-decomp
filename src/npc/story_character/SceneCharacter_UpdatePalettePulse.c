#include "npc/story_character/state_actor.h"
#include "engine/engine.h"
s32 Mod(s32, s32);
void Palette_InterpolateBanks(void *dst, void *from, void *to, s32 t, s32 shift);
/* Pulse the palette between the 0x680 and 0x2E80 banks: 64 frames each way. */
void SceneCharacter_UpdatePalettePulse(void *context, struct StateActor *actor)
{
    s32 t;
    if (actor->enter) {
        actor->enter = 0;
        actor->phase = 0;
        actor->stateFrame = 0;
        actor->done = 1;
    }
    t = Mod(actor->stateFrame, 128);
    if (t <= 64) {
        u8 *bank = gPaletteBankData;
        u8 *from = bank + 0x680, *to = bank + 0x2e80;
        Palette_InterpolateBanks(actor->overlayDrawable[0].palette, from, to, t, 6);
    } else {
        u8 *bank = gPaletteBankData;
        u8 *from = bank + 0x2e80, *to = bank + 0x680;
        Palette_InterpolateBanks(actor->overlayDrawable[0].palette, from, to, t - 64, 6);
    }
}
