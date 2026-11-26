#include "engine/shared/global.h"
#include "engine/shared/render.h"
void PauseMenu_RefreshStatusPanel(void *);
void Hud_OnUnpausedRefresh(void *p)
{
    if (!(gEnginePauseFlags & 1))
        PauseMenu_RefreshStatusPanel(p);
}
