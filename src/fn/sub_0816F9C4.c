#define gSceneInputKeys gUnk_03005260
#include "global.h"
s32 MessageWindow_Clear(void);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
s32 MessageWindow_SetMessageTable(const void *);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_ShowMessage(s32);
#include "scene_hud_display.h"
extern u16 gUnk_03005260[];
s32 TextWindow_SetScript(s32, const u8 *);
void TextWindow_SetBounds(s32, s32, s32, s32, s32);
void TextWindow_StartPlayback(s32);
void TextWindow_Clear(s32);
void Inventory_GetEquipmentDescriptor(s32, void *);
void Presentation_ApplyIndexedGraphicData(s32, void *, void *);
void SceneHud_RefreshActionIcons(void *);
void SceneHud_UpdateActionSelection(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0816CDB8(void *, int, int);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_ShowActionPreview(void *);
void Hud_SaveGridSelection(void *);
void InventoryPanel_ShowStatusIcons(void *);
void MenuDisplay_UpdateActionPrompt(void *, int);
void InventoryPanel_DrawStatComparison(void *, int, int, int, int, int);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void SceneHud_UpdateTimedMessage(struct SceneHudState *p)
{
    u8 text[32];
    u8 *q;
    int z;
    if (p->actionTransferDelay == 0) {
        MessageWindow_Clear();
        MessageWindow_SetBounds(0, 18, 30, 2);
        Inventory_GetEquipmentDescriptor(
            *(s16 *)((u8 *)gGameState + 0x68 + 2 * *(s16 *)((u8 *)gGameState + 0x50)), text);
        TextWindow_SetBounds(p->timedMessageWindow, 0, 16, 30, 2);
        TextWindow_SetScript(p->timedMessageWindow, p->timedMessageScript);
        Presentation_ApplyIndexedGraphicData(p->timedMessageWindow, text, p->unknownA44);
        TextWindow_StartPlayback(p->timedMessageWindow);
    }
    if (p->actionTransferDelay == 8) {
        MessageWindow_SetMessageTable(p->actionTextScript);
        MessageWindow_SetDisplayMode(1);
        MessageWindow_ShowMessage(120);
    }
    p->actionTransferDelay++;
    z = 0;
    if (p->actionTransferDelay > 89 || (p->actionTransferDelay > 15 && (gUnk_03005260[1] & 1))) {
        TextWindow_Clear(p->timedMessageWindow);
        MessageWindow_SetBounds(0, 16, 30, 2);
        SceneHud_RefreshActionIcons(p);
        Hud_SetInteractionCallback(p, SceneHud_UpdateActionSelection, 1);
        q = (u8 *)p + 0x4384;
        sub_0816CDB8(q, 1, 0);
        SceneHud_GetActionIconPosition((u8 *)p + 0x1620, ((u8 *)p)[0x4387]);
        SceneHud_ShowActionPreview(p);
        Hud_SaveGridSelection(q);
        InventoryPanel_ShowStatusIcons(p);
        MenuDisplay_UpdateActionPrompt(p, 4);
        InventoryPanel_DrawStatComparison(p, 0, 0, 0, 1, z);
        MenuDisplay_SetPaletteOverrides(p, 1, 0, 0, z);
    }
}
