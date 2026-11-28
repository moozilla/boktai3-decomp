#include "global.h"
#include "engine/video/sprite_drawable_api.h"

void Effect_Remove(struct SpriteDrawable *sprite)
{
    if (sprite->linked)
        Effect_UnlinkFromList(sprite, sprite->list);
}
