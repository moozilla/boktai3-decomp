#define Presentation_ApplyPackedGraphicData sub_08174578
#define Presentation_ApplyGraphicSettings sub_081744AC
#include "global.h"
void Inventory_InitEquipmentEntryFromDescriptor(const void *, void *);
void sub_081744AC(u8 *, u32);
void sub_08174578(const void *data, u32 target)
{
    u8 settings[16];
    Inventory_InitEquipmentEntryFromDescriptor(data, settings);
    sub_081744AC(settings, target);
}
