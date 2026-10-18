#include "sprite_effect_controller.h"
#define SpriteDrawable_SelectTileFrame sub_08217EEC
#define gSpriteShapeDimensions         gUnk_086140EC
extern const u16 gUnk_086140EC[];
void sub_08217EEC(struct SpriteEffectTileDrawable *sprite,
                  const struct SpriteEffectTileResource *resource, u32 frame)
{
    sprite->width = gUnk_086140EC[resource->shape];
    sprite->height = gUnk_086140EC[resource->shape] >> 8;
    sprite->attributes = ((resource->shape & 3) << 14) | ((resource->shape & 12) << 28);
    if (resource->flags & 0x10)
        sprite->attributes |= 0x2000;
    sprite->tile = (sprite->width >> 3) * (sprite->height >> 3) * frame + resource->tileBase;
}
