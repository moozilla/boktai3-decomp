#include "global.h"
#include "scene_hud_display.h"
#define SceneHud_InitGameplayResources sub_081789C8
#define SceneHud_InitActionIcons       sub_08178920
void Hud_SetupBackgroundLayer(void *, u32, int, int, u32);
void sub_08167E64(struct SceneHudState *);
void sub_08167F1C(struct SceneHudState *);
void SceneHud_InitActionIcons(struct SceneHudState *);
void sub_081789C8(struct SceneHudState *hud)
{
    struct SceneInputResource *resource;
    hud->controlMode = 0;
    hud->panelResource = (u32)ScriptProc_Start(0xC091, 0xA597);
    hud->panelConfig = (u32)ScriptProc_Start(0x92B3, 0xEFDA) + 0x1B4;
    Hud_SetupBackgroundLayer(hud, hud->panelResource, 0, 0, hud->panelConfig);
    hud->panelInitialized = 0;
    resource = ScriptProc_Start(0xCB05, 0x5D04);
    if (!resource)
        return;
    *(struct SceneInputResource *)hud->motionResource = *resource;
    ResourceHeader_Relocate(hud->motionResource, resource);
    resource = ScriptProc_Start(0xCB05, 0x530D);
    if (!resource)
        return;
    *(struct SceneInputResource *)hud->widgetResource = *resource;
    ResourceHeader_Relocate(hud->widgetResource, resource);
    sub_08167E64(hud);
    sub_08167F1C(hud);
    SceneHud_InitActionIcons(hud);
}
