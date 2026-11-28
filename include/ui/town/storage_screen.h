#ifndef GUARD_STORAGE_SCREEN_H
#define GUARD_STORAGE_SCREEN_H

#include "ui/widgets/scene_input.h"

struct StorageScreen {
    u8 task[0x18];
    struct AnimSprite controls[8];
    struct AnimSprite firstSlots[16], secondSlots[16];
    struct AnimSprite indicators[5];
    struct AnimSprite categorySlots[8];
    struct AnimSprite labels[3];
    struct AnimSpriteResource commonResource, storageResource;
    struct SceneInputPair markers, glyphs;
    const void *text; /* message table */
    u8 unknown1744[8];
    u8 interactionMode, choiceState;
    u16 timer;
    u32 mode;
    u8 category, categoryCount, emptyCarried;
    s8 emptyStored;
    u8 categoryKinds[8];
    u8 gridKind, cursorX, cursorY, selection, savedX, savedY, savedSelection;
    u8 unknown1767[0x29];
    u8 slots[32];
    u8 unknown17B0[0x14];
    u8 choiceRepeat;
    u8 unknown17C5[3];
    u16 cursorPalette[16];
    u8 paletteFrame;
    u8 unknown17E9[3];
    u32 completionScript;
    u32 updateCallback;
    u32 state;
};

/* The background, palette and message table pointers overlap the end of the glyph pair. */
struct StorageTextView {
    u8 unknown0[0x172C];
    void *background, *paletteResource;
    const void *itemText, *equipmentText, *consumableText;
};

#endif
