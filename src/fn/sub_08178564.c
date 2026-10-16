#include "global.h"
extern u8 *gUnk_02000214; /* field HUD/menu manager */
void Hud_SetMode(u32);
/* Script: clears HUD byte +0xA6D and sets HUD mode 7 (used when the message
   window closes). */
void Hud_CloseMessageFromScript(void)
{
    if (gUnk_02000214) {
        gUnk_02000214[0xA6D] = 0;
        Hud_SetMode(7);
    }
}
