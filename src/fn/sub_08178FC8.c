#include "global.h"
#include "ram.h"
#include "player.h"
void Bg_SetPriority(u32, u32);
extern u8 gUnk_08E60F0C[];
void Inventory_InitEquipmentEntryFromDescriptor(u8 *, u8 *);
u32 Script_SeekToKeyword(u32);
u32 Script_GetValue(void);
void SceneHud_ReadPageVisibility(u8 *);
void Hud_ReadScriptQOption(u8 *);
void Hud_LoadLayoutResources(u8 *);
void SelectionPanel_RefreshAttachedPanel(u8 *);
void sub_08176F70(u8 *);
u32 TextWindow_Create(u32, u32, u32, u32);
void sub_08167FC0(u8 *);
void FieldHud_RefreshModeEight(void);
void Hud_SetUpdateCallback(u8 *, void (*)(void));
void InventoryPanel_InitCallbackGroups(u8 *);
u32 Hud_InitAlternateLayout(u8 *s)
{
    u32 zero, value;
    u16 one;
    u32 offset;
    s32 negative;
    void (*callback)(void);
    s[0x4CD8] = (zero = 0);
    Bg_SetPriority(0, 0);
    offset = 0xA5C;
    *(u16 *)(s + offset) = zero;
    value = gSceneHudMode;
    s[0x1414] = value;
    value = (*(u32 *)gPlayers);
    *(u32 *)(s + 0xA40) = value;
    one = 1;
    Inventory_InitEquipmentEntryFromDescriptor(gUnk_08E60F0C, s + 0x4884);
    if (Script_SeekToKeyword(0x4E))
        s[0xA5E] = Script_GetValue();
    else
        s[0xA5E] = 8;
    SceneHud_ReadPageVisibility(s);
    Hud_ReadScriptQOption(s);
    Hud_LoadLayoutResources(s);
    SelectionPanel_RefreshAttachedPanel(s);
    sub_08176F70(s);
    *(u32 *)(s + 0x48F0) = 0;
    s[0x48E7] = 0;
    *(u32 *)(s + 0x4878) = 0;
    *(u32 *)(s + 0x4880) = 0;
    *(u32 *)(s + 0x48C4) = TextWindow_Create(6, 4, 10, 2);
    *(u32 *)(s + 0x48C8) = TextWindow_Create(4, 6, 22, 6);
    *(u32 *)(s + 0x48CC) = TextWindow_Create(2, 3, 10, 2);
    *(u32 *)(s + 0x48D4) = TextWindow_Create(2, 5, 21, 2);
    *(u32 *)(s + 0x48DC) = TextWindow_Create(2, 7, 11, 2);
    *(u32 *)(s + 0x48D0) = TextWindow_Create(17, 3, 15, 2);
    *(u32 *)(s + 0x48D8) = TextWindow_Create(13, 9, 10, 2);
    *(u32 *)(s + 0x48E0) = TextWindow_Create(13, 12, 12, 2);
    sub_08167FC0(s);
    callback = FieldHud_RefreshModeEight;
    Hud_SetUpdateCallback(s, callback);
    InventoryPanel_InitCallbackGroups(s);
    Hud_SetUpdateCallback(s, callback);
    s[0xB01] = 2;
    offset = 0xB02;
    s[offset] = 0xFF;
    *(s8 *)(s + 0xB03) = (negative = -one);
    offset -= 0xA3;
    *(s8 *)(s + offset) = negative;
    gFieldHud = s;
    return 0;
}
