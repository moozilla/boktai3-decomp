#include "render.h"
#include "scene_panel.h"
void ScenePanel_UpdateBackgroundEffect(struct ScenePanel *, s32, s32, s32, s32);
void ScenePanel_DrawOverlay(struct ScenePanel *, s32, s32, s32, s32);
void ScenePanel_DrawBackgroundLayers(struct ScenePanel *panel)
{
    s32 x = gUnk_03005418[0] + gCamera.offsetX;
    s32 y = gUnk_03005418[1] + gCamera.offsetY;
    s32 tileX = truncate3(x - 128), tileY = truncate3(y - 128) & ~1;
    x -= 120;
    y -= 80;
    ScenePanel_UpdateBackgroundEffect(panel, tileX, tileY, x, y);
    ScenePanel_DrawOverlay(panel, tileX, tileY, x, y);
}
