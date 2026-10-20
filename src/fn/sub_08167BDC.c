#include "global.h"

/* HUD interpretation and descriptive names are tentative. */
struct HudPanel {
    u8 pad0[0x428];
    u16 a, b;
};
struct Hud {
    u8 pad0[0xA40];
    struct HudPanel *panel;
    u8 padA44[0x1A];
    u8 mode;
};
void Hud_DrawHealthMeter(struct Hud *, s32, s32, s32, s32);
void sub_08164480(struct Hud *, s32, s32, s32, s32, s32, s32);
void StatDisplay_DrawGauge(struct Hud *, s32, s32);
void sub_08164864(struct Hud *, s32, s32, s32);
void Hud_UpdateTimedStatusIcon(struct Hud *);
void Hud_UpdateTransientStatGauge(struct Hud *);
void Hud_DrawStoredEnergyGauge(struct Hud *, s32);
void SelectionPanel_UpdateBlink(struct Hud *);
void PauseMenu_UpdateSelectedEquipmentIcons(struct Hud *);
void Hud_UpdateCountdownDigits(struct Hud *);
void Hud_RefreshActivePanel(struct Hud *hud)
{
    if (hud->panel && hud->mode != 7 && hud->mode <= 2) {
        s32 one = 1;
        Hud_DrawHealthMeter(hud, 0, 4, 1, one);
        sub_08164480(hud, 0, 4, 2, hud->panel->a, hud->panel->b, one);
        StatDisplay_DrawGauge(hud, 3, 3);
        sub_08164864(hud, 0, 9, 0x12);
        Hud_UpdateTimedStatusIcon(hud);
        if (hud->mode == 0)
            Hud_UpdateTransientStatGauge(hud);
        else
            Hud_DrawStoredEnergyGauge(hud, hud->mode);
        SelectionPanel_UpdateBlink(hud);
        PauseMenu_UpdateSelectedEquipmentIcons(hud);
        Hud_UpdateCountdownDigits(hud);
    }
}
