#include "global.h"
extern u8 *gUnk_02000214; /* field HUD/menu manager */
void sub_08177C9C(u8 *);
/* Script: applies the -o element visibility list (sub_08177C9C) unless the
   HUD is busy (+0xA5C). */
void Hud_SetLayoutFromScript(void)
{
    if (gUnk_02000214 && !*(u16 *)(gUnk_02000214 + 0xA5C))
        sub_08177C9C(gUnk_02000214);
}
