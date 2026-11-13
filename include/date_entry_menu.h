#ifndef GUARD_DATE_ENTRY_MENU_H
#define GUARD_DATE_ENTRY_MENU_H

#include "scene_input.h"
struct DateMenuWidget {
    struct SceneInputWidget sprite;
    struct SceneInputResource resource;
    u32 unknown80[2];
};

struct DateMenu {
    struct Task task;
    struct DateMenuWidget widgets[4];
    struct SceneInputWidget digits[12];
    struct SceneInputResource digitResource;
    s32 selection, timer, fadeShift, state, result;
    s32 year, month, day, hour, minute;
    s32 oldYear, oldMonth, oldDay, oldHour, oldMinute;
    s32 repeatTimer;
    u16 *savedPalette;
    u16 colors[16];
    u8 pulseState[4];
};

typedef char DateMenuSizeCheck[sizeof(struct DateMenu) == 0x740 ? 1 : -1];

struct DateMenuPalette {
    u32 header[5];
    u16 colors[256];
};
extern const struct ContactPosition gUnk_0824FCCC[12];
s32 SunClock_GetPackedDate(void);
void Menu_SetTextRect(struct SceneInputWidget *, const struct ContactPosition *);
s32 Mod(s32, s32);
s32 Div(s32, s32);
s32 Clock_DaysInMonth(s32, s32);
void SunClock_SetDateTime(s32, s32, s32, s32, s32, s32, s32);
void Time_CalculateSunriseSunset(s32, s32, s32);
void Menu_StartFadeOut(s32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void Menu_StartFadeIn(u32);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void DateMenu_HideWidgets(struct DateMenu *p);
void DateMenu_InitializeDigits(struct DateMenu *menu);
void DateMenu_LoadCurrentDate(struct DateMenu *p);
void DateMenu_UpdateCursor(struct DateMenu *p);
void DateMenu_UpdateDigits(struct DateMenu *p);
void DateMenu_HandleInput(struct DateMenu *p);
void DateMenu_Update(struct DateMenu *p);
void DateMenu_Destroy(struct DateMenu *p);
s32 DateMenu_Initialize(struct DateMenu *menu);
void DateMenu_SetParametersFromScript(void);
void *DateMenu_Create(void);
#endif
