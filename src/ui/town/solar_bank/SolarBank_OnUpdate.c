#include "global.h"
#include "ui/town/solar_bank_panel.h"
void Callback_InvokeActor(void *, void (*)(void));
s32 SolarBank_OnUpdate(struct SolarBankPanel *panel)
{
    Callback_InvokeActor(panel, panel->state);
    return 0;
}
