#include "global.h"
#include "ui/town/solar_bank_panel.h"
void Sound_PlaySong(u32);
void Anim_Step(void *, void *);
void SolarBank_BeginSelection(struct SolarBankPanel *);
void SolarBank_UpdateOpening(struct SolarBankPanel *panel)
{
    if (panel->timer == 0x20)
        Sound_PlaySong(0x2be);
    Anim_Step(&panel->label, &panel->labelData);
    panel->timer++;
    if (panel->timer > 0x68)
        SolarBank_BeginSelection(panel);
}
