#include "global.h"
#define gHudManager gUnk_02000214
struct HudManager {
    u8 pad0[0xA5E];
    u8 mode;
    u8 padA5F[0x19];
    u16 value;
    u16 limit;
    u16 displayTimer;
    u8 effect;
};
extern struct HudManager *gUnk_02000214;
void Hud_UpdateCountdownDisplay(s32 value, s32 limit, u32 effect)
{
    struct HudManager *p = gUnk_02000214;
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
