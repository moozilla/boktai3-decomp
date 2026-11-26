#include "engine/shared/global.h"
#include "engine/shared/sprite_drawable_api.h"
struct BriefSpriteEffect {
    u8 unknown0[0x744];
    const struct SpriteEffectTileResource *resource;
    struct SpriteDrawable sprite;
    u8 active, tick, firstFrame;
};
void SpriteEffect_StartBriefAnimation(struct BriefSpriteEffect *p,
                                      const struct ContactPosition *position, s32 alternate)
{
    struct SpriteDrawable *sprite = &p->sprite;
    if (alternate)
        p->firstFrame = 0x48;
    else
        p->firstFrame = 4;
    SpriteDrawable_SelectTileFrame(sprite, p->resource, p->firstFrame);
    sprite->position = *position;
    sprite->flags &= ~1;
    p->active = 1;
    p->tick = 0;
}
