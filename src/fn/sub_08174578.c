#include "global.h"
void Inventory_InitEquipmentEntryFromDescriptor(const void *, void *);
void Presentation_ApplyGraphicSettings(u8 *, u32);
void Presentation_ApplyPackedGraphicData(const void *data, u32 target)
{
    u8 settings[16];
    Inventory_InitEquipmentEntryFromDescriptor(data, settings);
    Presentation_ApplyGraphicSettings(settings, target);
}
