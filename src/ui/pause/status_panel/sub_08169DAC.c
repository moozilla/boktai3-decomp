#include "global.h"
#include "engine/video/render.h"
void PauseMenu_RefreshStatusPanel(void *);
void Hud_OnUnpausedRefresh(void *p)
{
    if (!(gEnginePauseFlags & 1))
        PauseMenu_RefreshStatusPanel(p);
}
