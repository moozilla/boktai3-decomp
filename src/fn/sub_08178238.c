#include "global.h"
#include "ram.h"
#define gHudManager gFieldHud
struct HudManager {
    u8 pad0[0xA5E];
    u8 mode;
    u8 padA5F[0x19];
    u16 value;
    u16 limit;
    u16 displayTimer;
    u8 effect;
};
void Hud_UpdateCountdownDisplay(s32 value, s32 limit, u32 effect)
{
    struct HudManager *p = gFieldHud;
    if (p && p->mode == 0) {
        if (value < 0)
            p->value = 0;
        else
            p->value = value;
        p->limit = limit;
        p->displayTimer = 300;
        if (effect)
            p->effect = 6;
        else
            p->effect = 0;
    }
}
