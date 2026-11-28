#include "ui/town/shop_panel.h"
void Shop_PopulateItemGrid(struct ShopPanel *panel)
{
    s32 row;
    s32 column;
    s32 slot;
    s32 kind;
    AnimSprite_SelectGlyph(&panel->display.heading, &panel->display.widgetResource, 0x63);
    for (row = 0; row < 4; row++) {
        for (column = 0; column < 4; column++) {
            slot = row * 4 + column;
            kind = Inventory_GetItemId(slot);
            if (kind >= 0) {
                ((struct ShopFlagsView *)panel)->widgets[slot + 5].flags &= ~1;
                Anim_LoadKeyframe(&panel->display.primary[slot], &panel->display.gridResource,
                                  (u16)Selection_GetDisplayFrame(kind), 1);
            } else
                ((struct ShopFlagsView *)panel)->widgets[slot + 5].flags |= 1;
            SceneHud_SetKeyframePaletteBank(&panel->display.primary[slot], 0x373);
        }
    }
    for (row = 0; row < 2; row++) {
        for (column = 0; column < 4; column++) {
            slot = row * 4 + column;
            kind = panel->catalog[slot + panel->page * 8];
            if (kind >= 0) {
                s32 currency;
                ((struct ShopFlagsView *)panel)->widgets[slot + 21].flags &= ~1;
                Anim_LoadKeyframe(&panel->display.secondary[slot], &panel->display.gridResource,
                                  (u16)Selection_GetDisplayFrame(kind), 1);
                currency = gGameState->unk7B0;
                if (currency < (s32)SceneItemValue_GetItemBaseValue(kind))
                    SceneHud_SetKeyframePaletteBank(&panel->display.secondary[slot], 0x374);
                else
                    SceneHud_SetKeyframePaletteBank(&panel->display.secondary[slot], 0x373);
            } else
                ((struct ShopFlagsView *)panel)->widgets[slot + 21].flags |= 1;
        }
    }
    for (slot = 0; slot < 8; slot++)
        ((struct ShopFlagsView *)panel)->widgets[slot + 41].flags |= 1;
}
