#include "global.h"
#include "engine/ram.h"
/* Hud_ReapplyCurrentMode: Reads the current HUD mode and applies it again through Hud_SetMode */
void Hud_SetMode(u32);
/* Re-applies the current HUD mode (+0xA5E). */
void Hud_ReapplyCurrentMode(void)
{
    if (gFieldHud)
        Hud_SetMode(gFieldHud[0xA5E]);
}
