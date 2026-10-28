#include "global.h"
#include "solar_bank_panel.h"
#include "input.h"
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
s32 Anim_Step(void *, void *);
void Sound_PlaySong(u32);
void SolarBank_BeginClosing(void *);
void SolarBank_BeginAmountEntry(void *);
void SolarBank_UpdateSelection(struct SolarBankPanel *panel)
{

    u32 pressed;
    pressed = gPadState[1] & 1;
    if (pressed) {
        if (panel->selection == 0)
            goto cancel;
        Sound_PlaySong(0xdd);
        SolarBank_BeginAmountEntry(panel);
    } else if ((pressed = gPadState[1] & 2)) {
    cancel:
        Sound_PlaySong(0xde);
        SolarBank_BeginClosing(panel);
    } else {
        pressed = gPadState[1] & 0x30;
        if (pressed) {
            Sound_PlaySong(0xdc);
            panel->selection = 1 - panel->selection;
            if (panel->selection) {
                Anim_LoadKeyframe(&panel->prompt, panel->animData, 0x4b, 1);
            } else {
                Anim_LoadKeyframe(&panel->prompt, panel->animData, 0x4c, 1);
            }
        }
    }
    Anim_Step(&panel->label, panel->labelData);
}
