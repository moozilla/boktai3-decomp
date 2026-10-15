// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_states.h"
#define SceneInventory_BeginPurchaseConfirmation sub_0806167C
void SceneInventory_BeginPurchaseConfirmation(struct SceneInventoryControlView *panel)
{
    SceneInput_SaveSelection(&panel->grid);
    panel->interactionFlags = 0x14;
    switch (panel->category) {
        case 0:
            SceneInputPanel_ShowDescription(panel, 0, panel->equipmentText, 0);
            break;
        case 1: {
            u32 textOffset = (u32)&panel->itemText - (u32)panel;
            const void *message = *(const void **)((u32)panel + textOffset);
            u8 *saved = &panel->grid.savedSelection;
            register s32 index asm("r0");
            s8 *entry;
            s32 item;
            textOffset += 0x1C;
            index = *(u8 *)((u32)panel + textOffset);
            index = index * 8 - 16;
            index += *saved;
            entry = ((struct SceneInventoryCatalogView *)panel)->entries;
            entry += index;
            asm("" : "+r"(entry) : "r"(textOffset));
            item = *entry;
            SceneInputPanel_ShowDescription(panel, 0, message, item);
            break;
        }
        case 2: {
            register u32 textOffset asm("r1") = (u32)&panel->consumableText - (u32)panel;
            register const void **textSlot asm("r0");
            const void *message;
            u32 scratch;
            u8 *saved;
            register u32 pageOffset asm("r5");
            register s32 index asm("r0");
            s8 *entry;
            s32 item;
            textSlot = (const void **)((u32)panel + textOffset);
            message = *textSlot;
            scratch = (u32)&panel->grid.savedSelection - (u32)panel;
            saved = (void *)((u32)panel + scratch);
            pageOffset = (u32)&panel->page - (u32)panel;
            index = *(u8 *)((u32)panel + pageOffset);
            index = index * 8 - 16;
            asm("" : "+r"(index) : "r"(scratch));
            index += *saved;
            scratch = (u32)((struct SceneInventoryCatalogView *)panel)->entries - (u32)panel;
            asm("" : "+r"(scratch));
            entry = (void *)((u32)panel + scratch);
            entry += index;
            item = *entry;
            SceneInputPanel_ShowDescription(panel, 0, message, item);
            break;
        }
    }
    sub_080613BC(panel, 2);
    {
        register u32 promptOffset asm("r5") = (u32)&panel->display.prompt - (u32)panel;
        asm("" : "+r"(promptOffset));
        SceneInput_BeginChoice((void *)((u32)panel + promptOffset), &panel->display.widgetResource,
                               &panel->choiceValue, 1, &panel->choiceBlink);
    }
    sub_0806130C(panel, (void (*)(struct SceneInventoryControlView *))sub_080623C0);
}
