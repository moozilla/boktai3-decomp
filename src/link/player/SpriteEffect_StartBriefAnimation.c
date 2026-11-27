#include "global.h"
#include "engine/video/sprite_drawable_api.h"
#include "link/player/brief_sprite_effect.h"
void SpriteEffect_StartBriefAnimation(struct PlayerBriefSpriteEffect *p,
                                      const struct ContactPosition *position, s32 alternate)
{
    struct SpriteDrawable *sprite = &p->sprite;
    if (alternate)
        p->firstFrame = 0x48;
    else
        p->firstFrame = 4;
    SpriteDrawable_SelectTileFrame(sprite, p->graphics, p->firstFrame);
    sprite->position = *position;
    sprite->flags &= ~1;
    p->active = 1;
    p->tick = 0;
}
