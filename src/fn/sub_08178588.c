#include "global.h"
struct HudPanel {
    u8 pad0[0x428];
    u16 a, b;
};
struct Hud {
    u8 pad0[0xA40];
    struct HudPanel *panel;
};
extern struct Hud *gUnk_02000214; /* field HUD/menu manager */
void sub_08164738(struct Hud *, s32, s32);
void sub_08164438(struct Hud *, s32, s32, s32, s32);
void sub_0816475C(struct Hud *, s32, s32);
void sub_08164480(struct Hud *, s32, s32, s32, s32, s32, s32);
/* Registry routine B0AA: HUD element 0 setup calls (sub_08164738,
   sub_08164438, sub_0816475C, then sub_08164480 with the panel's two values at
   +0x428); tentative. */
void Hud_ShowPanelFromScript(void)
{
    struct Hud *hud = gUnk_02000214;
    if (hud) {
        s32 one;
        sub_08164738(hud, 0, 1);
        one = 1;
        sub_08164438(hud, 0, 4, 1, one);
        sub_0816475C(hud, 0, 2);
        sub_08164480(hud, 0, 4, 2, hud->panel->a, hud->panel->b, one);
    }
}
