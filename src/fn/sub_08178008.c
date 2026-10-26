#include "global.h"
#include "ram.h"
struct Hud {
    u8 pad0[0x18];
    void *field18;
    u8 pad1C[0x10];
    s32 field2C;
};
void Hud_SetupBackgroundLayer(struct Hud *, void *, s32, s32, s32);
void Hud_RefreshSpriteFromScript(void)
{
    struct Hud *hud = gFieldHud;
    if (hud)
        Hud_SetupBackgroundLayer(hud, hud->field18, 0, 0, hud->field2C);
}
