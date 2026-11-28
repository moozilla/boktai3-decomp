#include "global.h"
#include "ui/town/solar_bank_panel.h"
void SolarBank_SetState(struct SolarBankPanel *panel, void (*state)(void))
{
    panel->state = state;
    panel->timer = 0;
}
