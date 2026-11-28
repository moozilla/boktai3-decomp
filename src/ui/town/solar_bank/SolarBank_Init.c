#include "global.h"

#include "engine/engine.h"
#include "ui/town/solar_bank_panel.h"
void SpriteMenu_InitBackground(struct SolarBankPanel *);
void SpriteMenu_LoadPalette(struct SolarBankPanel *);
void SolarBank_InitWidgets(struct SolarBankPanel *);
void SpriteMenu_InitText(struct SolarBankPanel *);
void SolarBank_BeginOpening(struct SolarBankPanel *);
/* Sets up the Solar Bank screen (four sub-inits), keeps -e at +0x6FC and
   runs SolarBank_BeginOpening; called by SolarBank_Create (tentative). */
u32 SolarBank_Init(struct SolarBankPanel *panel)
{
    u32 e;
    SpriteMenu_InitBackground(panel);
    SpriteMenu_LoadPalette(panel);
    SolarBank_InitWidgets(panel);
    SpriteMenu_InitText(panel);
    if ((e = Script_SeekToKeyword('e')) != 0)
        panel->completionScript = Script_GetValue();
    else
        panel->completionScript = e;
    SolarBank_BeginOpening(panel);
    return 0;
}
