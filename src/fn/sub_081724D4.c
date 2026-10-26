#define GameState_GetActionDescriptionIndex SolarFruit_GetInventorySlot
#define gAnimationPaletteData               gPaletteBankData
#include "global.h"
#include "scene_hud_display.h"
#include "game_state.h"
s32 GameState_GetUnlockEntry(s32);
s32 SolarFruit_GetInventorySlot(s32);
void SceneHud_RefreshEquippedActionIcons(struct SceneHudState *p)
{
    s32 slot, unlock, action, entry, description;
    struct SceneInputWidget *w;
    void *resource;
    if ((s16)gUnk_02000710->unk50 <= 7)
        slot = 0;
    else
        slot = (s16)gUnk_02000710->unk50 - 8;
    unlock = ((s16 *)((u8 *)gUnk_02000710 + 0x78))[slot];
    action = ((s16 *)((u8 *)gUnk_02000710 + 0x90))[gUnk_02000710->actionAuxiliarySlot];
    if (unlock == -1 || action == -1) {
        {
            struct SceneInputWidget *a = &p->comparisonUnlockWidget;
            a->flags |= 1;
        }
        {
            struct SceneInputWidget *a = &p->comparisonActionWidget;
            a->flags |= 1;
        }
    } else {
        entry = GameState_GetUnlockEntry(unlock);
        description = SolarFruit_GetInventorySlot(action);
        w = &p->comparisonUnlockWidget;
        resource = p->comparisonResource;
        Anim_LoadKeyframe(w, resource, (u16)(entry + 1), 0);
        w->flags &= ~1;
        if (description <= 5)
            Anim_LoadKeyframe(&p->comparisonActionWidget, resource, 0, 0);
        else
            Anim_LoadKeyframe(&p->comparisonActionWidget, resource, 1, 0);
        {
            struct SceneInputWidget *a = &p->comparisonActionWidget;
            a->flags &= ~1;
        }
        if (description <= 5)
            CpuSet(gPaletteBankData + ((0x349 + description) << 5), p->comparisonPalette,
                   0x4000008);
        else
            CpuSet(gPaletteBankData + ((0x18A + description) << 5), p->comparisonPalette,
                   0x4000008);
        p->comparisonActionWidget.paletteBank = 0xB000;
        p->comparisonActionWidget.palette = p->comparisonPalette;
    }
}
