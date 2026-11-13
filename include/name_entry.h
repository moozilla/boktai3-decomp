#ifndef GUARD_NAME_ENTRY_H
#define GUARD_NAME_ENTRY_H
#include "scene_input.h"
struct NameEntryWidget {
    struct SceneInputWidget sprite;
    struct SceneInputResource resource;
    u32 unknown80[2];
};
struct NameEntryParameters {
    const u8 *keyboardScript, *alternateKeyboardScript;
    u32 confirmScript, cancelScript;
    s32 mode, fadeShift;
};
struct NameEntryPalette {
    u32 header[5];
    u16 colors[256];
};
struct NameEntryTask {
    struct Task task;
    struct NameEntryWidget widgets[6];
    s32 characterPage, selection, position;
    void *bg;
    u16 verticalHistory, horizontalHistory;
    s32 verticalRepeat, horizontalRepeat;
    s16 state, result;
    s32 timer, fadeShift;
    u32 slot;
    u8 name[0x10], unknown384[0x10];
    const u8 *initialName;
    u16 *savedCursorPalette, *savedKeyPalette;
    u16 keyColors[16], cursorColors[16];
    u8 keyPulse, cursorPulse, unknown3E2[2];
};
extern const u8 gUnk_0824FCBC[], gUnk_0824FCC4[];
void memcpy(void *, const void *, u32);
s32 Mod(s32, s32);
u8 *PlayerName_GetBuffer(void);
void Menu_DrawText(u32, u32, const u8 *);
void Menu_EraseRect(u32, u32, u32, u32);
void Menu_SetBackgroundMap(void *, u32);
void Menu_StartFadeIn(u32);
void Menu_StartFadeOut(s32);
const u8 *Script_ReadString(const u8 *);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void NameEntry_CopyInitialName(struct NameEntryTask *p, u8 *out);
void NameEntry_CopyName(const void *src, void *dest);
void NameEntry_DrawName(const void *src);
void NameEntry_UpdateCursor(struct NameEntryTask *p);
void NameEntry_MoveCursorUp(struct NameEntryTask *p);
void NameEntry_MoveCursorDown(struct NameEntryTask *p);
void NameEntry_MoveCursorRight(struct NameEntryTask *p);
void NameEntry_MoveCursorLeft(struct NameEntryTask *p);
u32 NameEntry_AppendChar(struct NameEntryTask *p);
void NameEntry_ToggleCharacterPage(struct NameEntryTask *s);
s32 NameEntry_EraseCharacter(struct NameEntryTask *p);
void NameEntry_Update(struct NameEntryTask *p);
void NameEntry_Destroy(struct NameEntryTask *p);
s32 NameEntry_Initialize(struct NameEntryTask *menu);
void NameEntry_SetParametersFromScript(void);
struct NameEntryTask *NameEntry_Create(void);
#endif
