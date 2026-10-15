#include "scene_panel.h"
#define ScenePanel_DrawBackgroundLayers sub_08058FF8
void sub_08058700(struct ScenePanel *, s32, s32, s32, s32);
void sub_08058BD8(struct ScenePanel *, s32, s32, s32, s32);
void ScenePanel_DrawBackgroundLayers(struct ScenePanel *panel)
{
    s32 x = gUnk_03005418[0] + gUnk_030042A0.offsetX;
    s32 y = gUnk_03005418[1] + gUnk_030042A0.offsetY;
    s32 tileX = truncate3(x - 128), tileY = truncate3(y - 128) & ~1;
    x -= 120;
    y -= 80;
    sub_08058700(panel, tileX, tileY, x, y);
    sub_08058BD8(panel, tileX, tileY, x, y);
}
