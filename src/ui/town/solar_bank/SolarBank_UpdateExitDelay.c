#include "global.h"
#include "ui/town/solar_bank_panel.h"
void Script_Call(u32, u32);
void Task_Kill(void *);
void SolarBank_UpdateExitDelay(struct SolarBankPanel *panel)
{
    panel->timer++;
    if (panel->timer > 0x1f) {
        u32 script = panel->completionScript;
        if (script) {
            Script_Call(script, 0);
            Task_Kill(panel);
        }
    }
}
