#ifndef GUARD_DIRECTIONAL_SPRITE_STATE_H
#define GUARD_DIRECTIONAL_SPRITE_STATE_H
#include "global.h"
#include "display_sequence.h"

union DirectionalSpritePosition {
    struct {
        u16 x, height, depth, padding;
    } axis;
    u32 words[2];
};
struct DirectionalSprite {
    u32 flags;
    u8 unknown4[2], angle, priority, scaleX, scaleY;
    u8 unknownA[6];
    u16 frame;
    u8 unknown12[0xa];
    union DirectionalSpritePosition position;
    u8 unknown24[8];
};
/* Player.directionalEffect stores this sprite, resource and playback state. */
struct DirectionalSpriteEffect {
    struct DirectionalSprite sprite;
    u8 resourceHeader[6];
    u16 spriteResource;
    u8 unknown34[4];
    u32 variantResource;
    u8 unknown3C[8];
    u32 unknown44;
    struct DisplaySequenceState animation;
    void *sequenceResource;
    s16 velocityX;
    u8 unknown5E[2];
    s16 velocityY;
    u8 unknown62[2];
    u8 active, unknown65[3];
};
#endif
