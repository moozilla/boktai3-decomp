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
void sub_081647A4(struct Hud *, s32, s32);
void sub_08164864(struct Hud *, s32, s32, s32);
void sub_08165020(struct Hud *);
void sub_08164DC0(struct Hud *);
void sub_08164C40(struct Hud *, s32);
void SelectionPanel_UpdateBlink(struct Hud *);
void sub_08167910(struct Hud *);
void sub_08165254(struct Hud *);
void Hud_RefreshActivePanel(struct Hud *hud)
{
    if (hud->panel && hud->mode != 7 && hud->mode <= 2) {
        s32 one = 1;
        Hud_DrawHealthMeter(hud, 0, 4, 1, one);
        sub_08164480(hud, 0, 4, 2, hud->panel->a, hud->panel->b, one);
        sub_081647A4(hud, 3, 3);
        sub_08164864(hud, 0, 9, 0x12);
        sub_08165020(hud);
        if (hud->mode == 0)
            sub_08164DC0(hud);
        else
            sub_08164C40(hud, hud->mode);
        SelectionPanel_UpdateBlink(hud);
        sub_08167910(hud);
        sub_08165254(hud);
    }
}
