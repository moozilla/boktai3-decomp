#ifndef GUARD_ANIMATION_WIDGET_H
#define GUARD_ANIMATION_WIDGET_H
#include "engine/video/anim_sprite.h"
#include "engine/video/render.h"
#include "gba/gba.h"
struct WidgetGlyphEntry {
    u16 kind, resourceIndex, width, height, originX, originY, centerX, centerY;
    u32 tileOffset;
};
s32 Anim_SelectWidgetGlyph(struct AnimSprite *, struct AnimSpriteResource *, u16);
s32 Anim_InitPositionedWidget(struct AnimSprite *, void *, u16, u32, u8, u8, u8,
                              const struct ContactPosition *);
void Anim_Remove(struct AnimSprite *);
#endif
