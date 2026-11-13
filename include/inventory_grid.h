#ifndef GUARD_INVENTORY_GRID_H
#define GUARD_INVENTORY_GRID_H

#include "global.h"

struct InventoryGridVector {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));

struct InventoryNumberDigit {
    u32 flags;
    u8 unknown04[0x14];
    struct InventoryGridVector position;
    u8 unknown20[8];
};

struct InventoryGridElement {
    u32 flags;
    u8 unknown04[0x2E];
    u16 value;
    u8 unknown34[0x2C];
};

/* The element table is an overlapping view of the Scene HUD's widget storage. */
struct InventoryGrid {
    union {
        struct {
            u8 unknown0000[0xA4E];
            u16 phaseCounter;
            u8 unknown0A50[0x1620 - 0xA50];
            s16 slotPosition[4];
            u8 unknown1628[0x2AA8 - 0x1628];
            u32 motionFlags;
            u8 unknown2AAC[0x4384 - 0x2AAC];
            u8 selectionState[3];
            u8 selectedSlot;
            u8 unknown4388[2];
            u8 selectedItemSlot;
            u8 unknown438B[0x4860 - 0x438B];
            u8 itemStorage;
            u8 unknown4861[5];
            s16 appliedValue;
            u8 unknown4868[0x4CDC - 0x4868];
            void *digitResource;
            struct InventoryNumberDigit digits[4];
        } state;
        struct {
            u8 unknown0000[0x1428];
            struct InventoryGridElement elements[266];
        } elementView;
    } view;
};

#endif
