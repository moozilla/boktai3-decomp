#include "global.h"
#include "ram.h"

void Hud_SetMode(u32);
/* Re-applies the HUD mode saved by Hud_SaveMode (+0xA5F), if any, and
   clears the saved copy (-1). */
void Hud_RestoreMode(void)
{
    u8 *hud = gFieldHud;
    if (hud) {
        s8 *saved = (s8 *)(hud + 0xA5F);
        if (*saved >= 0) {
            Hud_SetMode(*saved);
            *(u8 *)saved = 0xFF;
        }
    }
}
