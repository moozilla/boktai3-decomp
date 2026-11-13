#ifndef GUARD_PASSWORD_MENU_H
#define GUARD_PASSWORD_MENU_H
#include "scene_input.h"
#include "menu_parameters.h"
struct PasswordWidget {
    struct SceneInputWidget sprite;
    struct SceneInputResource resource;
    u32 unknown80[2];
};
struct PasswordRows {
    u8 rows[4][256];
};
struct PasswordMenu {
    struct Task task;
    struct PasswordWidget widgets[6];
    u32 unknown348;
    s32 selection, position;
    void *background;
    u16 verticalHistory, horizontalHistory;
    s32 verticalRepeat, horizontalRepeat;
    s16 state, result;
    s32 timer, fadeShift;
    struct PasswordRows text;
    s32 selections[24];
    u16 *savedKeyPalette, *savedCursorPalette;
    u16 keyColors[16], cursorColors[16];
    u8 keyPulse, cursorPulse, unknown81A[2];
};
struct PasswordDisplay {
    struct Task task;
    struct SceneInputWidget widget;
    struct SceneInputResource resource;
    s16 state, frame;
    void *background;
    s32 fadeTimer, fadeShift;
    struct PasswordRows text;
    u8 encoded[24];
    s32 windows[3];
};
struct PasswordMenuPalette {
    u32 header[5];
    u16 colors[256];
};
void PasswordMenu_SaveSelections(struct PasswordMenu *p);
void PasswordMenu_LoadSelections(struct PasswordMenu *p);
void PasswordMenu_ResetSelections(struct PasswordMenu *p);
void PasswordMenu_JoinTextRows(struct PasswordRows *text, u8 *out);
void PasswordMenu_UpdateKeyboardCursor(struct PasswordMenu *p);
void PasswordMenu_MoveCursorUp(struct PasswordMenu *p);
void PasswordMenu_MoveCursorDown(struct PasswordMenu *p);
void PasswordMenu_MoveCursorLeft(struct PasswordMenu *p);
void PasswordMenu_MoveCursorRight(struct PasswordMenu *p);
void PasswordMenu_BuildLabels(struct PasswordMenu *p);
void PasswordMenu_RedrawSelection(struct PasswordMenu *p, s32 n);
void PasswordMenu_UpdateEntryCursor(struct PasswordMenu *p);
s32 PasswordMenu_AppendSelection(struct PasswordMenu *p);
s32 PasswordMenu_EraseSelection(struct PasswordMenu *p);
void PasswordMenu_Update(struct PasswordMenu *p);
void PasswordMenu_Destroy(struct PasswordMenu *p);
s32 PasswordMenu_Initialize(struct PasswordMenu *s);
void PasswordMenu_SetParametersFromScript(void);
void *PasswordMenu_Create(void);
u32 PasswordDisplay_Update(struct PasswordDisplay *p);
s32 PasswordDisplay_Destroy(struct PasswordDisplay *p);
s32 PasswordDisplay_Initialize(struct PasswordDisplay *p);
void PasswordDisplay_SetParametersFromScript(void);
void *PasswordDisplay_Create(void);
extern const u8 gUnk_0824FD2C[];
extern const u8 gUnk_0824FD34[];
void memcpy(void *, const void *, u32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawText(s32, s32, const u8 *);
extern u8 gUnk_0824FD3C[];
extern u8 gUnk_0824FD44[];
extern struct GameState *gGameState;
void Menu_StartFadeOut(s32);
u8 *PlayerName_GetBuffer(void);
s32 Password_DecodeEntry(void *, u32, u32, void *);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void Menu_StartFadeIn(u32);
extern const u8 gUnk_086136D8[];
extern const u8 gUnk_086136E4[];
void EncodedRecord_EncodeFields(void *);
s32 TextWindow_Create(u32, u32, u32, u32);
void TextWindow_SetString(int, int, void *);
s32 TextWindow_SetScript(s32 id, const u8 *script);
s32 TextWindow_Refresh(s32 id, s32 index);
void TextWindow_StartPlayback(s32);
#endif
