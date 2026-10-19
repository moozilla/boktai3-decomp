#include "global.h"
#include "scene_inventory_panel.h"
void Inventory_GetEquipmentDescriptor(s32, void *);
void Presentation_ApplyGraphicData(void *, void *);
void Presentation_ApplyPackedGraphicData(const void *, u32);
u32 Presentation_SelectGraphicResource(void *);
struct StoragePanel {
    u8 unknown0[0x1098];
    struct SceneInputWidget banner;
    u8 unknown10F8[0x1538 - 0x10F8];
    struct SceneInputResource gridResource;
    u8 unknown1558[0x1738 - 0x1558];
    u32 equipmentText;
    u8 unknown173C[8];
    u8 graphicTarget[0x1f];
    u8 savedSelection;
    u8 unknown1764[0x2c];
    u8 items[1];
};
struct EquipmentDescriptor {
    u32 words[8];
};
void Storage_HideItemDetails(struct StoragePanel *);
void Storage_ShowEquipmentDescription(struct StoragePanel *panel)
{
    struct EquipmentDescriptor descriptor;
    struct SceneValueEquipment *entry = 0;
    struct EquipmentDescriptor *data;
    if (panel->savedSelection <= 31) {
        entry = Inventory_GetEquipmentEntry(panel->items[panel->savedSelection]);
        if (entry->kind == 0xFF)
            goto hide;
        Inventory_GetEquipmentDescriptor(panel->items[panel->savedSelection], &descriptor);
        data = &descriptor;
    } else
        data = 0;
    if (!data) {
    hide:
        Storage_HideItemDetails(panel);
    } else {
        struct SceneInputWidget *banner = &panel->banner;
        banner->flags &= ~1;
        if (entry) {
            struct SceneInputResource *resource = &panel->gridResource;
            Anim_LoadKeyframe(banner, resource, (u16)Presentation_SelectGraphicResource(entry), 1);
        }
        MessageWindow_SetMessageTable((void *)panel->equipmentText);
        MessageWindow_SetBounds(2, 16, 28, 2);
        Presentation_ApplyGraphicData(&descriptor, panel->graphicTarget);
        Presentation_ApplyPackedGraphicData(&descriptor, panel->equipmentText);
    }
}
