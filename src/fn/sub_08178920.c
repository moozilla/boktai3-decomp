#include "global.h"
#include "scene_hud_display.h"
#define SceneHud_SetWidgetPaletteBank PauseMenu_SetSpritePalette
void SceneHud_SetWidgetPaletteBank(struct SceneInputWidget *, u16);
void SceneHud_InitActionIcons(struct SceneHudState *hud)
{
    struct SceneInputPackedPosition pos;
    pos.x = 8;
    pos.y = 0x88;
    pos.z = 0;
    sub_0821983C(&hud->primaryWidgets[0], hud->motionResource, 0xB6, 0x10, 0, 0, 0x3C, &pos);
    SceneHud_SetWidgetPaletteBank(&hud->primaryWidgets[0], 1);
    pos.x = 0xD8;
    pos.y = 0x88;
    pos.z = 0;
    sub_0821983C(&hud->secondaryWidgets[0], hud->motionResource, 0, 0x10, 0, 0, 0x3C, &pos);
}
