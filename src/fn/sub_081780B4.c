#include "global.h"
#include "ram.h"
/* Hud_ReapplyCurrentMode: Reads the current HUD mode and applies it again through Hud_SetMode */
#define Hud_ReapplyCurrentMode sub_081780B4
void Hud_SetMode(u32);
/* Re-applies the current HUD mode (+0xA5E). */
void sub_081780B4(void)
{
    if (gFieldHud)
        Hud_SetMode(gFieldHud[0xA5E]);
}
