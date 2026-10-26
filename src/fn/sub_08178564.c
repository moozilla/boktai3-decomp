#include "global.h"
#include "ram.h"
void Hud_SetMode(u32);
/* Script: clears HUD byte +0xA6D and sets HUD mode 7 (used when the message
   window closes). */
void Hud_CloseMessageFromScript(void)
{
    if (gFieldHud) {
        gFieldHud[0xA6D] = 0;
        Hud_SetMode(7);
    }
}
