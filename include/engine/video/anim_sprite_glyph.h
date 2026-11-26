#ifndef GUARD_ANIM_SPRITE_GLYPH_H
#define GUARD_ANIM_SPRITE_GLYPH_H
#include "engine/video/anim_sprite.h"
#include "engine/video/render.h"
#include "gba/gba.h"
struct AnimSpriteGlyph {
    u16 kind, resourceIndex, width, height, originX, originY, centerX, centerY;
    u32 tileOffset;
};
s32 AnimSprite_SelectGlyph(struct AnimSprite *, struct AnimSpriteResource *, u16);
s32 AnimSprite_InitAtPosition(struct AnimSprite *, void *, u16, u32, u8, u8, u8,
                              const struct ContactPosition *);
void Anim_Remove(struct AnimSprite *);
#endif
