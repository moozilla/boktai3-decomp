#ifndef GUARD_SCENE_INVENTORY_LIFECYCLE_H
#define GUARD_SCENE_INVENTORY_LIFECYCLE_H
#include "scene_inventory_tail.h"
struct SceneInventoryLifecycle {
    u8 unknown0[0x1AAC];
    u8 purchased, sold;
    u8 unknown1AAE[0x1AEE - 0x1AAE];
    u16 frame;
    u8 unknown1AF0[0x1CF4 - 0x1AF0];
    u32 completionScript;
    s32 (*action)(struct SceneInventoryTail *);
    s32 (*callback)(struct SceneInventoryTail *);
};
/* Store the argument count without reading the unspecified upper-half padding. */
struct SceneInventoryScriptArgs {
    u32 purchased, sold;
};
struct SceneInventoryScriptCall {
    u32 argumentCount : 16;
    u32 reserved : 16;
    struct SceneInventoryScriptArgs *args;
};
#endif
