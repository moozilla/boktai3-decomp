#ifndef GUARD_INVENTORY_PANEL_H
#define GUARD_INVENTORY_PANEL_H

#include "level_advance_display.h"

struct InventoryPanelElement {
    u32 flags;
    u8 unknown4[0x5C];
};

struct InventoryPanelCallbackMetadata {
    u8 active, age;
    u8 unknown2[2];
    void *callback;
    u8 unknown8[0x38];
};

struct InventoryPanelCallback {
    struct LevelAdvanceParticle particle;
    void *callback;
};

struct InventoryPanelCallbackGroup {
    void *context;
    u8 active, initialized;
    u8 unknown6[2];
    union {
        struct InventoryPanelCallback entries[8];
        struct {
            u8 unknown8[0x38];
            struct InventoryPanelCallbackMetadata metadata[8];
        } metadataView;
    } callbacks;
};

struct InventoryPanel {
    u8 unknown0[0x2EC8];
    struct InventoryPanelElement selected[3];
    u8 unknown2FE8[0x4450 - 0x2FE8];
    struct InventoryPanelCallbackGroup groups[2];
};

#endif
