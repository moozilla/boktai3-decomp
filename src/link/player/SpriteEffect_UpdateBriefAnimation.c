#include "global.h"
#include "link/player/brief_sprite_effect.h"
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void SpriteEffect_UpdateBriefAnimation(struct PlayerBriefSpriteEffect *p)
{
    if (p->active) {
        if (++p->tick > 5) {
            p->sprite.flags |= 1;
            p->active = 0;
        } else
            SpriteDrawable_SelectTileFrame(&p->sprite.flags, p->graphics,
                                           p->firstFrame + (p->tick >> 2));
    }
}
