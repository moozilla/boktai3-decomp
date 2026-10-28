// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
#include "engine_globals.h"
void Bg_SetPriority(u32, u32);
extern u32 gUnk_02000580;
extern u8 gUnk_08E60F0C[];
extern u8 *gGameState;
void Inventory_InitEquipmentEntryFromDescriptor(u8 *, u8 *);
u32 Script_SeekToKeyword(u32);
u32 Script_GetValue(void);
void SceneHud_ReadScriptOptions(u8 *);
void SceneHud_ReadPageVisibility(u8 *);
void Hud_ReadScriptQOption(u8 *);
void Hud_LoadLayoutResources(u8 *);
void SelectionPanel_RefreshAttachedPanel(u8 *);
void sub_08176F70(u8 *);
u32 TextWindow_Create(u32, u32, u32, u32);
void sub_08167FC0(u8 *);
void PauseMenu_RefreshStatusPanel(u8 *);
void Hud_RefreshActivePanel(u8 *);
void Hud_OnUnpausedRefresh(void);
void FieldHud_RefreshModeEight(void);
void Hud_UpdateEquipmentPrompt(void);
void Hud_SetUpdateCallback(u8 *, void (*)(void));
void InventoryPanel_InitCallbackGroups(u8 *);
void SceneHud_RebuildTertiaryOptions(u8 *);
u32 DarkLoan_CreateHiddenDisplay(u32, u32, u32, u32);

static inline void MinusOne(u8 *base, u32 offset)
{
    u8 *p = base + offset;
    register s32 one asm("r0") = 1;
    *p = -one;
}
// confidence=low: initializes the scene HUD and its script-configured panels.
#define SceneHud_InitFromScript sub_08178C80
u32 SceneHud_InitFromScript(u8 *s)
{
    u32 zero, value;
    u32 r;
    u32 z;
    int one;
    u32 offset;
    s[0x4CD8] = (zero = 0);
    Bg_SetPriority(0, 0);
    offset = 0xA5C;
    *(u16 *)(s + offset) = zero;
    value = gSceneHudMode;
    s[0x1414] = value;
    value = gUnk_02000580;
    *(u32 *)(s + 0xA40) = value;
    Inventory_InitEquipmentEntryFromDescriptor(gUnk_08E60F0C, s + 0x4884);
    if (Script_SeekToKeyword(0x4E))
        s[0xA5E] = Script_GetValue();
    else
        s[0xA5E] = 0;
    if (s[0xA5E] != 8)
        SceneHud_ReadScriptOptions(s);
    s[0xA6C] = 1;
    SceneHud_ReadPageVisibility(s);
    Hud_ReadScriptQOption(s);
    Hud_LoadLayoutResources(s);
    SelectionPanel_RefreshAttachedPanel(s);
    sub_08176F70(s);
    if ((r = Script_SeekToKeyword(0x64)))
        *(u32 *)(s + 0x48F0) = Script_GetValue();
    else
        *(u32 *)(s + 0x48F0) = r;
    if ((r = Script_SeekToKeyword(0x56)))
        s[0x48E7] = Script_GetValue();
    else
        s[0x48E7] = r;
    if ((r = Script_SeekToKeyword(0x4F)))
        s[0x48E6] = Script_GetValue();
    else
        s[0x48E6] = r;
    if ((r = Script_SeekToKeyword(0x52)))
        *(u32 *)(s + 0x4878) = Script_GetValue();
    else
        *(u32 *)(s + 0x4878) = r;
    if ((r = Script_SeekToKeyword(0x44)))
        *(u32 *)(s + 0x4880) = Script_GetValue();
    else
        *(u32 *)(s + 0x4880) = r;
    if (!(gEngineFlags & 0x1000)) {
        *(u32 *)(s + 0x48C4) = TextWindow_Create(6, 4, 10, 2);
        *(u32 *)(s + 0x48C8) = TextWindow_Create(4, 6, 22, 6);
        *(u32 *)(s + 0x48CC) = TextWindow_Create(2, 3, 10, 2);
        *(u32 *)(s + 0x48D4) = TextWindow_Create(2, 5, 21, 2);
        *(u32 *)(s + 0x48DC) = TextWindow_Create(2, 7, 11, 2);
        *(u32 *)(s + 0x48D0) = TextWindow_Create(17, 3, 15, 2);
        *(u32 *)(s + 0x48D8) = TextWindow_Create(13, 9, 10, 2);
        *(u32 *)(s + 0x48E0) = TextWindow_Create(13, 12, 12, 2);
    }
    sub_08167FC0(s);
    if (gEngineFlags & 0x1000) {
        PauseMenu_RefreshStatusPanel(s);
        Hud_SetUpdateCallback(s, Hud_OnUnpausedRefresh);
    } else if (s[0xA5E] == 8) {
        Hud_SetUpdateCallback(s, FieldHud_RefreshModeEight);
    } else {
        Hud_RefreshActivePanel(s);
        Hud_SetUpdateCallback(s, Hud_UpdateEquipmentPrompt);
    }
    InventoryPanel_InitCallbackGroups(s);
    {
        u8 *field = s + 0xB01;
        z = 0;
        *field = 2;
    }
    s[0xB02] = 0xFF;
    MinusOne(s, 0xB03);
    one = 1;
    offset = 0xA5F;
    MinusOne(s, offset);
    *(u32 *)(s + 0x4928) = z;
    gFieldHud = s;
    SceneHud_RebuildTertiaryOptions(s);
    if ((*(u32 *)(gGameState + 0x868) & 0x580) || (gEngineFlags & 0x3800))
        *(u32 *)(s + 0x48F4) = z;
    else
        *(u32 *)(s + 0x48F4) = DarkLoan_CreateHiddenDisplay(s[0x48E6], *(u32 *)(s + 0x48F0),
                                                            s[0x48E7], *(u32 *)(s + 0x58));
    return 0;
}
