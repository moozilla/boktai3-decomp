#include "global.h"
void Sound_PlaySong(u32);
int SceneHud_UpdateBinaryChoice(void *);
void SceneCatalog_SetDescription(u8 *, s32);
void SceneHud_SetPairedIcons(u8 *, u32, s32);
void Hud_SetInteractionCallback(u8 *, void (*)(void), u32);
void MenuDisplay_SetPaletteOverrides(u8 *, u32, u32, u32, u32);
void Presentation_UpdateAllocationSelection(void);
void InventoryGrid_UpdateModifierPreview(void);
void Presentation_CommitListValues(u8 *);
void Player_RefreshStatsAndVitals(void *);
void Presentation_DrawEntryCount(u8 *);
void InventoryGrid_LayoutStatMarkers(u8 *);
void Hud_DrawDefaultHealthMeter(u8 *, u32, u32, u32);
void Hud_DrawDefaultEnergyMeter(u8 *, u32, u32, u32, u32, u32);
void MenuSelection_ProcessConfirmation(u8 *s)
{
    s32 state = SceneHud_UpdateBinaryChoice(s);
    if (state == 0) {
        Sound_PlaySong(0xDE);
        SceneCatalog_SetDescription(s, s[0x48AA] + 20);
        SceneHud_SetPairedIcons(s, 1, -1);
        Hud_SetInteractionCallback(s, Presentation_UpdateAllocationSelection, 1);
        MenuDisplay_SetPaletteOverrides(s, 1, 0, 0, 0);
    } else if (state == 1) {
        u8 **p;
        s32 neg;
        u32 count;
        Sound_PlaySong(0xDD);
        Presentation_CommitListValues(s);
        p = (u8 **)(s + 0xA40);
        Player_RefreshStatsAndVitals(*p);
        Presentation_DrawEntryCount(s);
        InventoryGrid_LayoutStatMarkers(s);
        Hud_DrawDefaultHealthMeter(s, 1, 8, 8);
        Hud_DrawDefaultEnergyMeter(s, 1, 8, 10, *(u16 *)(*p + 0x428), *(u16 *)(*p + 0x42A));
        neg = -1;
        SceneHud_SetPairedIcons(s, 1, neg);
        count = *(u16 *)(s + 0x48A8);
        if (count == 0) {
            *(u32 *)(s + 0x1608) |= state;
            SceneCatalog_SetDescription(s, neg);
            Hud_SetInteractionCallback(s, InventoryGrid_UpdateModifierPreview, 0);
            MenuDisplay_SetPaletteOverrides(s, 0, 0, 0, count);
        } else {
            SceneCatalog_SetDescription(s, s[0x48AA] + 20);
            MenuDisplay_SetPaletteOverrides(s, 1, 0, 0, 0);
            Hud_SetInteractionCallback(s, Presentation_UpdateAllocationSelection, 0);
        }
    }
}
