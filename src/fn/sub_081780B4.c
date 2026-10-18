#include "global.h"
/* Hud_ReapplyCurrentMode: Reads the current HUD mode and applies it again through Hud_SetMode */
#define Hud_ReapplyCurrentMode sub_081780B4
extern u8 *gUnk_02000214; /* field HUD/menu manager */
void Hud_SetMode(u32);
/* Re-applies the current HUD mode (+0xA5E). */
void sub_081780B4(void)
{
    if (gUnk_02000214)
        Hud_SetMode(gUnk_02000214[0xA5E]);
}
