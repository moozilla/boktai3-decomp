#ifndef GUARD_MENU_DISPLAY_H
#define GUARD_MENU_DISPLAY_H
#include "scene_input.h"
#include "menu_parameters.h"
#include "script_utility.h"
struct SolarCalibrationWidget {
    struct SceneInputWidget sprite;
    struct SceneInputResource graphics;
    u32 unknown80[2];
};
struct SolarCalibrationMenu {
    struct Task task;
    void *backgroundMap;
    u32 unknown1C[2];
    s32 blinkTimer, pageTimer, state, page, timer, cancelState, fadeTimer, fadeSpeed;
    struct SolarCalibrationWidget widgets[3];
};
struct MenuDisplayPalette {
    u32 header[5];
    u16 colors[256];
};
s32 Mod(s32, s32);
s32 Solar_CaptureBaseline(void);
void Save_WriteHeader(void);
void MessageWindow_Clear(void);
void MessageWindow_ShowMessage(u32);
void MessageWindow_SetBounds(u32, u32, u32, u32);
void MessageWindow_SetPalette(u32);
void MessageWindow_SetMessageTable(const u8 *);
void MessageWindow_SetDisplayMode(u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void BackgroundMap_ClearTextRect(s32, s32, s32, s32);
const u8 *Text_FindChar(const u8 *, u8);
u16 Text_DecodeGlyphEscape(const u8 *);
void TextWindow_DrawNarrowGlyph(u32, s32, s32, u32);
void TextWindow_DrawWideGlyph(u32, s32, s32, u32);
u32 Text_GetWideGlyphCount(void);
void Bg_SetLayerMap(u32, void *, u32);
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void SolarCalibration_Update(struct SolarCalibrationMenu *s);
void SolarCalibration_Destroy(struct SolarCalibrationMenu *p);
s32 SolarCalibration_Initialize(struct SolarCalibrationMenu *s);
void SolarCalibration_SetParametersFromScript(void);
struct SolarCalibrationMenu *SolarCalibration_Create(void);
void Menu_EraseRect(s32 a, s32 b, s32 c, s32 d);
void Menu_DrawText(s32 left, s32 row, const u8 *s);
void Menu_DrawStringRef(s32 column, s32 row, const void *source, s32 index);
void Menu_SetTextRect(struct SceneInputWidget *widget, const struct ContactPosition *position);
void Menu_SetBackgroundMap(void *p, u32 b);
void Menu_StartFadeIn(u32 a);
void Menu_StartFadeOut(u32 a);
#endif
