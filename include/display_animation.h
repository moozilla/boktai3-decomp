#ifndef GUARD_DISPLAY_ANIMATION_H
#define GUARD_DISPLAY_ANIMATION_H
/* Tentative common display wrapper: kind 1 uses sequence sprites; kind 2 uses keyframes. */
#include "engine.h"
#include "display_sequence.h"
struct DisplayAnimationResource {
    u8 unknown0[6];
    u16 paletteIndex;
    u8 unknown8[4];
    const void *paletteData;
    u8 unknown10[0x18];
    u32 flags;
    u8 unknown2C[0x2C];
    void *sequence;
};
struct DisplayPosition {
    u32 words[2]; /* observed word-aligned copy; packed coordinates remain untyped */
};
struct DisplayAnimation {
    u32 kind;
    u32 index;
    u8 variant, flags, finished, unknownB;
    struct DisplayAnimationResource resource;
    u8 unknown68[0x24];
    struct DisplayPosition *position;
    u32 *displayFlags;
};
struct DisplaySequenceSprite {
    u32 flags;
    u8 unknown4[2];
    u8 attribute, priority, palette, paletteVariant;
    u8 unknownA[2];
    const u8 *resource;
    u16 frame;
    u8 frameIndex, elapsed, width, height, offsetX, offsetY;
    u32 attributes;
    u8 unknown1C[0x10];
};
struct DisplayKeyframeState {
    u8 unknown0[0x14];
    u16 frame;
    u8 unknown16[4];
    u8 priority;
    u8 unknown1B[2];
    u8 status;
    u8 unknown1E[0x16];
    u8 palette, paletteVariant;
    u16 attribute;
    u8 unknown38[2];
    u16 paletteIndex;
};
#endif
