#include "ui/town/shop_states.h"
void Shop_BeginPurchaseConfirmation(struct ShopControlView *panel)
{
    SceneInput_SaveSelection(&panel->grid);
    panel->interactionFlags = 0x14;
    switch (panel->category) {
        case 0:
            SceneInputPanel_ShowDescription(panel, 0, panel->equipmentText, 0);
            break;
        case 1: {
            const void *message = panel->itemText;
            u8 *saved = &panel->grid.savedSelection;
            s32 index = panel->page * 8 - 16;
            index += *saved;
            SceneInputPanel_ShowDescription(
                panel, 0, message, ((struct ShopCatalogView *)panel)->entries[index]);
            break;
        }
        case 2: {
            const void *message = panel->consumableText;
            u8 *saved = &panel->grid.savedSelection;
            s32 index = panel->page * 8 - 16;
            index += *saved;
            SceneInputPanel_ShowDescription(
                panel, 0, message, ((struct ShopCatalogView *)panel)->entries[index]);
            break;
        }
    }
    Shop_UpdateModePrompt(panel, 2);
    SceneInput_BeginChoice(&panel->display.prompt, &panel->display.widgetResource,
                           &panel->choiceValue, 1, &panel->choiceBlink);
    Shop_SetControllerCallback(
        panel, (ShopCallback)Shop_TickPurchaseConfirmation);
}
