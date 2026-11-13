#ifndef GUARD_REGION_MENU_H
#define GUARD_REGION_MENU_H
#include "scene_input.h"
#include "menu_parameters.h"
#include "script_utility.h"
#include "sun_clock.h"
#include "session_save.h"
struct RegionEntry {
    s16 group;
    u16 name;
    s32 longitude, latitude;
};
struct RegionMenuWidget {
    struct SceneInputWidget sprite;
    struct SceneInputResource resource;
    u32 unknown80[2];
};
struct RegionMenu {
    struct Task task;
    u32 unknown18;
    s32 page, count;
    u8 unknown24[2];
    s8 selected;
    u8 pulse;
    s32 pageFirst[14];
    struct RegionMenuWidget widgets[4];
    struct RegionEntry entries[64];
    u16 *savedPalette;
    u16 palette[16];
    s32 state, confirmed, timer, fadeSpeed, skipped;
};
struct RegionMenuPalette {
    u32 header[5];
    u16 colors[256];
};
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawStringRef(s32, s32, const void *, s32);
void Menu_StartFadeIn(u32);
void Menu_StartFadeOut(s32);
void Menu_SetTextRect(void *, void *);
void Time_CalculateSunriseSunset(s32, s32, s32);
void Save_WriteHeader(void);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
s32 Script_ReadValue(const void *);
s32 Script_GetValueSafe(void);
void Time_ShowRegionMenu(struct RegionMenu *menu);
void Time_OnRegionListSelect(struct RegionMenu *menu);
void Time_HandleRegionKeypad(struct RegionMenu *menu);
void Time_DestroyRegionMenu(struct RegionMenu *p);
s32 Time_LoadRegionList(struct RegionMenu *menu);
void Time_SetRegionMenuParametersFromScript(void);
struct RegionMenu *Time_CreateRegionMenu(void);
void Time_OnLoadSave(void);
#endif
