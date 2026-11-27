#include "global.h"
void Hud_SetGridPosition(void *, int, int);
void PauseMenu_ResetItemCursor(u8 *p)
{
    Hud_SetGridPosition(p + 0x4384, 0, 0);
}
