#include "global.h"
#include "ram.h"
void SceneHud_ReadPageVisibility(u8 *);
/* Script: applies the -o element visibility list (SceneHud_ReadPageVisibility) unless the
   HUD is busy (+0xA5C). */
void Hud_SetLayoutFromScript(void)
{
    if (gFieldHud && !*(u16 *)(gFieldHud + 0xA5C))
        SceneHud_ReadPageVisibility(gFieldHud);
}
