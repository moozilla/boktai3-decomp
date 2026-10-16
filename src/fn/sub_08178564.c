#include "global.h"
extern u8 *gUnk_02000214; /* field HUD/menu manager */
void sub_08177DEC(u32);
/* Script: clears HUD byte +0xA6D and sets HUD mode 7 (used when the message
   window closes). */
void Hud_CloseMessageFromScript(void)
{
    if (gUnk_02000214) {
        gUnk_02000214[0xA6D] = 0;
        sub_08177DEC(7);
    }
}
