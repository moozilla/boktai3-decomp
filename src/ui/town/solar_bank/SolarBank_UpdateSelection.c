#include "global.h"
#include "ui/town/solar_bank_panel.h"
#include "gba/io_reg.h"
#include "engine/input.h"
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
s32 Anim_Step(void *, void *);
void Sound_PlaySong(u32);
void SolarBank_BeginClosing(void *);
void SolarBank_BeginAmountEntry(void *);
void SolarBank_UpdateSelection(struct SolarBankPanel *panel)
{
    if (gPadState[1] & A_BUTTON) {
        if (panel->selection != 0) {
            Sound_PlaySong(0xdd);
            SolarBank_BeginAmountEntry(panel);
        } else {
            Sound_PlaySong(0xde);
            SolarBank_BeginClosing(panel);
        }
    } else if (gPadState[1] & B_BUTTON) {
        Sound_PlaySong(0xde);
        SolarBank_BeginClosing(panel);
    } else if (gPadState[1] & (DPAD_LEFT | DPAD_RIGHT)) {
        Sound_PlaySong(0xdc);
        panel->selection = 1 - panel->selection;
        if (panel->selection)
            Anim_LoadKeyframe(&panel->prompt, &panel->animData, 0x4b, 1);
        else
            Anim_LoadKeyframe(&panel->prompt, &panel->animData, 0x4c, 1);
    }
    Anim_Step(&panel->label, &panel->labelData);
}
