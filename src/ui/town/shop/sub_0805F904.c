#include "global.h"
#include "ui/town/shop_panel.h"
#include "ui/town/equipment_descriptor.h"
void Inventory_GetEquipmentDescriptor(s32, void *);
void Shop_PrepareEquipmentDescriptor(struct ShopPanel *, struct EquipmentDescriptor *);
void Presentation_ApplyGraphicData(void *, void *);
void Presentation_ApplyGraphicSettings(void *, const void *);
void Presentation_ApplyPackedGraphicData(const void *, const void *);
u32 Presentation_SelectGraphicResource(void *);
void sub_0805F904(struct ShopPanel *panel)
{
    struct EquipmentDescriptor descriptor;
    struct SceneValueEquipment fallback;
    struct SceneValueEquipment *entry;
    struct EquipmentDescriptor *data;
    if (panel->selection <= 15) {
        entry = Inventory_GetEquipmentEntry(panel->selection);
        if (entry->kind == 0xFF) {
            data = 0;
        } else {
            Inventory_GetEquipmentDescriptor(panel->selection, &descriptor);
            data = &descriptor;
        }
    } else if (panel->selection <= 23) {
        s32 kind = panel->catalog[panel->selection - 16 + panel->page * 8];
        entry = 0;
        if (kind >= 0) {
            descriptor = *(struct EquipmentDescriptor *)Equipment_GetDefinition(kind);
            data = &descriptor;
            descriptor.modifierA = 0;
            descriptor.modifierB = 0;
            descriptor.modifierC = 0;
            Shop_PrepareEquipmentDescriptor(panel, data);
        } else {
            data = 0;
        }
    } else {
        data = 0;
        entry = 0;
    }
    if (!data) {
        panel->display.banner.flags |= 1;
        MessageWindow_Clear();
        ScrollingDialogue_ClearScript();
    } else {
        u32 resource;
        if (!entry) {
            fallback.condition = 500;
            resource = Presentation_SelectGraphicResource(&fallback);
        } else
            resource = Presentation_SelectGraphicResource(entry);
        panel->display.banner.flags &= ~1;
        Anim_LoadKeyframe(&panel->display.banner, &panel->display.gridResource, (u16)resource, 1);
        MessageWindow_SetMessageTable(panel->equipmentText);
        MessageWindow_SetBounds(2, 16, 28, 2);
        Presentation_ApplyGraphicData(&descriptor, panel->descriptionState);
        if (entry)
            Presentation_ApplyGraphicSettings(entry, panel->equipmentText);
        else
            Presentation_ApplyPackedGraphicData(&descriptor, panel->equipmentText);
    }
    SceneInputPanel_ShowItemComparison(panel->comparison[0], panel->comparison[1],
                                       &panel->display.widgetResource, data, -1);
    SceneInputPanel_ShowAttributeComparison(panel->comparison[2], panel->comparison[3],
                                            panel->comparison[4], &panel->display.widgetResource,
                                            -1);
}
