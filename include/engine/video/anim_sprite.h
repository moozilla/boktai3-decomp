#ifndef GUARD_SCENE_INPUT_WIDGET_H
#define GUARD_SCENE_INPUT_WIDGET_H

#include "scene/collision/contact_position.h"

struct AnimSpritePlayback {
    u8 priority, mode;
};

struct AnimSpriteGlyph;
struct AnimationKeyframe {
    u16 key, duration;
};
struct AnimationKeyframeSequence {
    u16 unknown0, frameCount;
    u32 offset;
};
/* The relocatable 32-byte glyph/keyframe resource header. */
struct AnimSpriteResource {
    u16 paletteIndex, glyphCount, sequenceCount, unknown6;
    u32 unknown8;
    struct AnimSpriteGlyph *glyphs;
    struct AnimationKeyframeSequence *sequences;
    u8 *tileBase;
    u8 *frameData;
    u8 *extraData;
};

struct AnimSprite {
    u16 glyphKind;
    u16 stamp; /* Copied from 03005238 on initialization; meaning tentative. */
    u8 active, unknown5; /* active: linked to an animation list */
    u16 value6;
    u32 flags;
    u16 elapsed, duration, durationScale; /* durationScale has six fractional bits (0x40 = 1x). */
    u16 sequenceId, frameIndex, frameCount, playbackFlags;
    u8 attribute1A, mode;
    u8 list, status;
    u16 unknown1E;
    struct ContactPosition position;
    u16 width, height, originX, originY, centerX, centerY;
    u8 scaleX, scaleY;
    u16 rotation; /* 0x36: angle copied by the thrown HUD icon. */
    u16 resourceIndex;
    u16 paletteBank;
    u32 value3C;
    s32 cachedFrame, cachedPalette;
    u16 *palette;
    u8 *tileData, *extraData;
    struct AnimationKeyframe *frames; /* sequence frame table at +0x54 */
    struct AnimSprite *previous, *next;
};

/* flags bit 0 hides the widget's sprite. */
static inline void AnimSprite_Hide(struct AnimSprite *animSprite)
{
    animSprite->flags |= 1;
}

static inline void AnimSprite_Show(struct AnimSprite *animSprite)
{
    animSprite->flags &= ~1;
}

#endif
