#ifndef GUARD_INCLUDE_UI_TOWN_EQUIPMENT_DESCRIPTOR_H
#define GUARD_INCLUDE_UI_TOWN_EQUIPMENT_DESCRIPTOR_H

#include "global.h"

struct EquipmentDescriptor {
    u32 prefix[3]; /* confidence=low */
    u8 modifierA; /* confidence=low */
    u8 padD[0x3];
    u8 modifierB; /* confidence=low */
    u8 pad11[0x3];
    u8 modifierC; /* confidence=low */
    u8 pad15[0xb];
};

typedef char EquipmentDescriptorSizeCheck[sizeof(struct EquipmentDescriptor) == 0x20 ? 1 : -1];

#endif
