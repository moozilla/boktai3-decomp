#ifndef GUARD_MENU_DISPLAY_H
#define GUARD_MENU_DISPLAY_H
#include "pause_menu.h"

struct MenuDisplay {
    struct SceneHudState hud;
    s8 consumableEntrySlots[4];
    s16 activePairIndices[2];
};
void MenuDisplay_SetPaletteOverrides(struct MenuDisplay *hud, s32 primary, s32 secondary,
                                     s32 alternate, s32 comparison);
void MenuDisplay_UpdateHighlightPalette(struct MenuDisplay *hud);
void MenuDisplay_LoadAdjustedDigits(s32 value, s32 adjustment, void *data, void *hundredsAnim,
                                    void *tensAnim, void *onesAnim);
void MenuDisplay_DrawStatComparison(struct MenuDisplay *hud, s32 col, s32 row);
s32 MenuDisplay_FindEntryIndex(struct MenuDisplay *hud, s32 entry, s32 group);
void MenuDisplay_InterpolateActivePairPositions(struct MenuDisplay *hud, s32 weight);
void MenuDisplay_BeginActionTransfer(struct MenuDisplay *hud, s32 mode);
u32 MenuDisplay_IsCategoryEntryEmpty(struct MenuDisplay *menu, u32 entry, u32 category);
u32 MenuDisplay_CanSwapCategoryEntries(void *hud, u32 first, u32 second, u32 category);
s32 MenuDisplay_GetCategoryEntryValue(u32 entry, s32 category);
s32 MenuDisplay_CountAvailableEntries(void *unused, s32 category);
s32 MenuDisplay_UpdateActionPrompt(struct MenuDisplay *menu, s32 group);
void MenuDisplay_SnapshotValues(struct MenuDisplay *hud);
void MenuDisplay_LoadDecimalDigits(struct MenuDisplay *hud, s32 value, s32 divisor, s32 frameBase,
                                   s32 firstWidget);
#endif
