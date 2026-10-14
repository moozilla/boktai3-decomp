#include "modifier_list.h"
extern u32 gUnk_030053F0;
void ModifierMenu_ValidateSavedSelection(struct ModifierMenu *);
void ModifierMenu_LoadSavedSelection(struct ModifierMenu *);
void ModifierMenu_LoadTextSources(struct ModifierMenu *);
void ModifierMenu_InitializePreview(struct ModifierMenu *);
void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);
void ModifierMenu_LoadBackground(struct ModifierMenu *, s32);
u32 sub_080407F0(struct ModifierMenu *);
void ModifierMenu_InitializeGraphics(struct ModifierMenu *);
void sub_08040ABC(struct ModifierMenu *);
void sub_0811D14C(void *, void *);
s32 sub_080413E0(struct ModifierMenu *);
void sub_08033568(void);
void sub_080414E4(struct ModifierMenu *);
s32 ModifierMenu_Initialize(struct ModifierMenu *menu)
{
    u32 mask;
    u32 flags;
    menu->unknown116 = 0;
    ModifierMenu_ValidateSavedSelection(menu);
    ModifierMenu_LoadSavedSelection(menu);
    mask = 0x800;
    flags = (gUnk_030053F4 | gUnk_030053F0) & mask;
    /* The masked value is zero or 0x800: negation extracts its nonzero flag. */
    menu->specialHeader = Script_GetKeywordValue('E', (0 - flags) >> 31);
    menu->script = (void *)Script_GetKeywordValue('e', 0);
    if (Script_GetKeywordValue('f', 1)) {
        menu->inputDelay = 32;
        sub_082279A8(2, 5, 4, 4, 4, 0xFFFF, 0);
    } else {
        menu->inputDelay = 0;
    }
    ModifierMenu_LoadTextSources(menu);
    ModifierMenu_LoadBackground(menu, 0);
    sub_080407F0(menu);
    ModifierMenu_InitializeGraphics(menu);
    sub_08040ABC(menu);
    sub_0811D14C((u8 *)menu + 0x3C, menu->tilemap);
    if (sub_080413E0(menu) < 0)
        return -1;
    sub_08033568();
    menu->dirtyFlags = -1;
    ModifierMenu_InitializePreview(menu);
    sub_080414E4(menu);
    return 0;
}
