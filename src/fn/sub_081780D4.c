#include "global.h"
extern u8 *gUnk_02000214; /* field HUD/menu manager */
void SceneHud_ReadPageVisibility(u8 *);
/* Script: applies the -o element visibility list (SceneHud_ReadPageVisibility) unless the
   HUD is busy (+0xA5C). */
void Hud_SetLayoutFromScript(void)
{
    if (gUnk_02000214 && !*(u16 *)(gUnk_02000214 + 0xA5C))
        SceneHud_ReadPageVisibility(gUnk_02000214);
}
