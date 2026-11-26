#include "engine/video/render.h"
#include "objects/pile_driver/pile_driver.h"
void PileDriver_UpdateBackgroundEffect(struct PileDriver *, s32, s32, s32, s32);
void PileDriver_DrawOverlay(struct PileDriver *, s32, s32, s32, s32);
void PileDriver_DrawBackgroundLayers(struct PileDriver *panel)
{
    s32 x = ((s16 *)gCameraOrigin)[0] + gCamera.offsetX;
    s32 y = ((s16 *)gCameraOrigin)[1] + gCamera.offsetY;
    s32 tileX = truncate3(x - 128), tileY = truncate3(y - 128) & ~1;
    x -= 120;
    y -= 80;
    PileDriver_UpdateBackgroundEffect(panel, tileX, tileY, x, y);
    PileDriver_DrawOverlay(panel, tileX, tileY, x, y);
}
