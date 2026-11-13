#ifndef GUARD_SPRITE_OAM_H
#define GUARD_SPRITE_OAM_H
#include "render.h"
#include "contact_position.h"
/* Drawable render-list object; list links are shared by the Effect API. */
struct SpriteDrawable {
    u32 flags;
    u8 linked, scaleX, scaleY, angle;
    s16 tile;
    u8 width, height;
    s8 offsetX, offsetY;
    u8 palette, priority;
    s8 depthOffset;
    u8 list;
    u16 unknown12;
    u32 attributes;
    struct ContactPosition position;
    struct SpriteDrawable *previous, *next;
};
void SpriteOam_QueueProjectedSprites(void);
void SpriteOam_QueueCameraRelativeSprites(void);
void SpriteOam_QueueFlaggedProjectedSprites(void);
void SpriteOam_QueueOverlayProjectedSprites(void);
#endif
