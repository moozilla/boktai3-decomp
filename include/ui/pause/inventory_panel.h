#ifndef GUARD_INVENTORY_PANEL_H
#define GUARD_INVENTORY_PANEL_H

#include "engine/video/sprite_drawable_api.h"

struct InventoryPanelElement {
    u32 flags;
    u8 unknown4[0x5C];
};

struct InventoryPanelCallback;

struct InventoryPanelCallbackState {
    u8 active, age;
    u8 unknown2[2];
    u32 (*callback)(struct InventoryPanelCallback *, void *);
};

struct InventoryPanelCallback {
    struct SpriteDrawable sprite;
    u8 unknown28[8];
    s16 velocityX, velocityY;
    u8 unknown34[4];
    struct InventoryPanelCallbackState state;
};

struct InventoryPanelCallbackGroup {
    void *context;
    u8 active, initialized;
    u8 unknown6[2];
    struct InventoryPanelCallback entries[8];
};

struct InventoryPanel {
    u8 unknown0[0x2EC8];
    struct InventoryPanelElement selected[3];
    u8 unknown2FE8[0x4450 - 0x2FE8];
    struct InventoryPanelCallbackGroup groups[2];
    u8 unknown4860[0x4CE0 - 0x4860];
    struct SpriteDrawable numberDigits[4];
};

u32 MenuParticle_UpdateMotion(struct InventoryPanelCallback *, void *);

typedef char InventoryPanelCallbackSizeCheck[sizeof(struct InventoryPanelCallback) == 0x40 ? 1 : -1];
typedef char InventoryPanelCallbackGroupSizeCheck[
    sizeof(struct InventoryPanelCallbackGroup) == 0x208 ? 1 : -1];

#endif
