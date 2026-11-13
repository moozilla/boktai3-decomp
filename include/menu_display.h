#ifndef GUARD_MENU_DISPLAY_H
#define GUARD_MENU_DISPLAY_H
#include "pause_menu.h"

struct MenuDisplay {
    struct SceneHudState hud;
    s8 consumableEntrySlots[4];
    s16 activePairIndices[2];
};
void MenuDisplay_SetPaletteOverrides(struct MenuDisplay *p, s32 a, s32 b, s32 c, s32 d);
void MenuDisplay_UpdateHighlightPalette(struct MenuDisplay *p);
void MenuDisplay_LoadAdjustedDigits(s32 value, s32 adjustment, void *data, void *hundredsAnim,
                                    void *tensAnim, void *onesAnim);
void MenuDisplay_DrawStatComparison(struct MenuDisplay *p, s32 col, s32 row);
s32 MenuDisplay_FindEntryIndex(struct MenuDisplay *p, s32 entry, s32 group);
void MenuDisplay_InterpolateActivePairPositions(struct MenuDisplay *p, s32 n);
void MenuDisplay_BeginActionTransfer(struct MenuDisplay *p, s32 mode);
u32 PauseMenu_IsCategoryEntryEmpty(struct MenuDisplay *s, u32 i, u32 k);
u32 MenuDisplay_CanSwapCategoryEntries(void *ctx, u32 a, u32 b, u32 c);
s32 MenuDisplay_GetCategoryEntryValue(u32 a, s32 b);
s32 MenuDisplay_CountAvailableEntries(void *unused, s32 category);
void MenuDisplay_SnapshotValues(struct MenuDisplay *panel);
void MenuDisplay_LoadDecimalDigits(struct MenuDisplay *p, s32 value, s32 divisor, s32 frameBase,
                                   s32 first);
#endif
