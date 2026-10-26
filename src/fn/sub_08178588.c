#include "global.h"
#include "ram.h"
struct HudPanel {
    u8 pad0[0x428];
    u16 a, b;
};
struct Hud {
    u8 pad0[0xA40];
    struct HudPanel *panel;
};
void Hud_DrawHealthLabel(struct Hud *, s32, s32);
void Hud_DrawHealthMeter(struct Hud *, s32, s32, s32, s32);
void Hud_DrawEnergyLabel(struct Hud *, s32, s32);
void sub_08164480(struct Hud *, s32, s32, s32, s32, s32, s32);
/* Registry routine B0AA: HUD element 0 setup calls (Hud_DrawHealthLabel,
   Hud_DrawHealthMeter, Hud_DrawEnergyLabel, then sub_08164480 with the panel's two values at
   +0x428); tentative. */
void Hud_ShowPanelFromScript(void)
{
    struct Hud *hud = gFieldHud;
    if (hud) {
        s32 one;
        Hud_DrawHealthLabel(hud, 0, 1);
        one = 1;
        Hud_DrawHealthMeter(hud, 0, 4, 1, one);
        Hud_DrawEnergyLabel(hud, 0, 2);
        sub_08164480(hud, 0, 4, 2, hud->panel->a, hud->panel->b, one);
    }
}
