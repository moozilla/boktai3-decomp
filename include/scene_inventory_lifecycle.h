#ifndef GUARD_SCENE_INVENTORY_LIFECYCLE_H
#define GUARD_SCENE_INVENTORY_LIFECYCLE_H
#include "global.h"
/* Narrow phase view; catalog/transition storage has other overlapping views. */
struct SceneInventoryLifecycleView {
    u8 unknown0[0x1A88];
    void *messageText, *equipmentText, *unknown1A90, *itemText, *consumableText;
    u8 unknown1A9C[8];
    u8 interactionFlags, unknown1AA5, mode, presentation, category;
    u8 unknown1AA9[2];
    u8 clockMode, unknown1AAC, unknown1AAD[3], page, pageCount, itemPageCount;
    u8 unknown1AB3[0x1AEE - 0x1AB3];
    u16 frame;
    u8 unknown1AF0[0x1CF8 - 0x1AF0];
    void (*action)(struct SceneInventoryLifecycleView *);
};
s32 sub_080632AC(struct SceneInventoryLifecycleView *);
s32 sub_080632EC(struct SceneInventoryLifecycleView *);
s32 sub_08063334(struct SceneInventoryLifecycleView *);
#endif
