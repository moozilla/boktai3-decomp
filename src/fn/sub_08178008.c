#include "global.h"
struct Hud {
    u8 pad0[0x18];
    void *field18;
    u8 pad1C[0x10];
    s32 field2C;
};
extern struct Hud *gUnk_02000214; /* field HUD/menu manager */
void sub_08163F14(struct Hud *, void *, s32, s32, s32);
/* Script: sub_08163F14 on the HUD with its +0x18 and +0x2C (tentative). */
void Hud_RefreshSpriteFromScript(void)
{
    struct Hud *hud = gUnk_02000214;
    if (hud)
        sub_08163F14(hud, hud->field18, 0, 0, hud->field2C);
}
