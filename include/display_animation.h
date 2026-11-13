#ifndef GUARD_DISPLAY_ANIMATION_H
#define GUARD_DISPLAY_ANIMATION_H
/* Tentative common display wrapper: kind 1 uses sequence sprites; kind 2 uses keyframes. */
#include "engine.h"
#include "display_sequence.h"
#include "display_keyframe.h"
#include "contact_position.h"
#include "scene_animation.h"
#include "scene_input_widget.h"
/* Resource header read through a sequence sprite (palette index and data). */
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
struct DisplaySequenceSprite {
    u32 flags;
    u8 unknown4[2];
    u8 attribute, priority, palette, paletteVariant; /* angle, priority, scale X/Y (legacy names) */
    u8 unknownA[2];
    const u8 *resource;
    u16 frame;
    u8 frameIndex, elapsed, width, height, offsetX, offsetY;
    u32 attributes;
    struct ContactPosition position;
    u8 unknown24[8];
};
/* Both modes begin at wrapper offset 0xC and occupy the same 0x80-byte storage. */
struct DisplayAnimationSequence {
    struct SceneAnimation graphics;
    struct DisplaySequenceSprite sprite;
    struct DisplaySequenceState playback;
    void *sequence;
};
struct DisplayAnimationKeyframes {
    struct SceneInputResource resource;
    struct SceneInputWidget widget;
};
union DisplayAnimationBody {
    struct DisplayAnimationSequence sequence;
    struct DisplayAnimationKeyframes keyframes;
};
struct DisplayAnimation {
    u32 kind;
    u32 index;
    u8 variant, flags, finished, unknownB;
    union DisplayAnimationBody resource;
    struct DisplayPosition *position;
    u32 *displayFlags;
};

#endif
