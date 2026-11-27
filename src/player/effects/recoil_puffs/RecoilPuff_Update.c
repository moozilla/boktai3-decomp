#include "global.h"
#include "player/effects/recoil_puff_sprite.h"
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void RecoilPuff_Update(struct RecoilPuffSprite *p)
{
    SpriteDrawable_SelectTileFrame(p, p->resource, (p->timer >> 1) + 12);
    p->position.y += 10;
    p->timer++;
    if (p->timer == 16) {
        p->flags |= 1;
        p->active = 0;
    }
}
