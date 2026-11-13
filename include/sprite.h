#ifndef GUARD_SPRITE_H
#define GUARD_SPRITE_H
#include "contact_position.h"
/* Legacy palette/paletteVariant names hold signed X/Y scales in the renderer.
 * unknown4 stores linked/visible; unknownA[0] selects the owning list. */
struct DisplaySequenceSprite {
    u32 flags;
    u8 unknown4[2];
    u8 attribute, priority, palette, paletteVariant;
    u8 unknownA[2];
    const u8 *resource;
    u16 frame;
    u8 frameIndex, elapsed, width, height, offsetX, offsetY;
    u32 attributes;
    struct ContactPosition position;
    struct DisplaySequenceSprite *previous, *next;
};
struct SpriteRenderPiece {
    u8 shape, unknown1;
    s8 x, y;
};
struct SpriteRenderFrame {
    u8 count, tileSize;
    u16 unknown2;
    u32 tileOffset, pieceOffset;
};
struct SpriteRenderResource {
    u8 flags, shape;
    u16 unknown2, unknown4, paletteKey;
    u8 width, height;
    s8 offsetX, offsetY;
    void *palette;
    u8 *tileData;
    struct SpriteRenderFrame *frames;
    u32 unknown18;
};
/* The eight-byte record beginning at DisplaySequenceSprite.frame. */
struct SpriteRenderAnimationState {
    s16 frame;
    u8 frameIndex, paletteOffset;
    u8 width, height;
    s8 x, y;
};
#endif
