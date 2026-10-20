#include "global.h"
extern u32 gUnk_0300523C;
void PauseMenu_RefreshStatusPanel(void *);
void Hud_OnUnpausedRefresh(void *p)
{
    if (!(gUnk_0300523C & 1))
        PauseMenu_RefreshStatusPanel(p);
}
