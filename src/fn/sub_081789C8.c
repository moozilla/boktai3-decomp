#include "global.h"
#include "scene_hud_display.h"
void Hud_SetupBackgroundLayer(void *, u32, int, int, u32);
void Hud_InitializePanelTransition(struct SceneHudState *);
void Hud_InitializeThreeIcons(struct SceneHudState *);
void SceneHud_InitActionIcons(struct SceneHudState *);
void SceneHud_InitGameplayResources(struct SceneHudState *hud)
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
    Hud_InitializePanelTransition(hud);
    Hud_InitializeThreeIcons(hud);
    SceneHud_InitActionIcons(hud);
}
