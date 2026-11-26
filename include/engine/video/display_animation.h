#ifndef GUARD_DISPLAY_ANIMATION_H
#define GUARD_DISPLAY_ANIMATION_H
/* Tentative common display wrapper: kind 1 uses sequence sprites; kind 2 uses keyframes. */
#include "engine/engine.h"
#include "engine/video/display_sequence.h"
#include "engine/video/display_keyframe.h"
#include "scene/collision/contact_position.h"
#include "scene/scene_animation.h"
#include "ui/widgets/scene_input_widget.h"
#include "engine/video/sprite.h"
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
