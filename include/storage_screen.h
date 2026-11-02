#ifndef GUARD_STORAGE_SCREEN_H
#define GUARD_STORAGE_SCREEN_H

#include "scene_input.h"

struct StorageScreen {
    u8 task[0x18];
    struct SceneInputWidget controls[8];
    struct SceneInputWidget firstSlots[16], secondSlots[16];
    struct SceneInputWidget indicators[5];
    struct SceneInputWidget categorySlots[8];
    struct SceneInputWidget labels[3];
    struct SceneInputResource commonResource, storageResource;
    struct SceneInputPair markers, glyphs;
    u8 unknown1740[0x14];
    u8 category, categoryCount, emptyCarried;
    s8 emptyStored;
    u8 categoryKinds[8];
    u8 gridKind, cursorX, cursorY, selection, savedX, savedY, savedSelection;
    u8 unknown1767[0x29];
    u8 slots[32];
};

#endif
