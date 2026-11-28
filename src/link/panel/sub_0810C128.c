#include "global.h"
#include "engine/video/anim_sprite.h"
s32 Anim_Step(void *, void *);

bool32 sub_0810C128(struct AnimSprite *sprite, void *resource)
{
    s32 result = Anim_Step(sprite, resource);

    if ((sprite->status & 1) || result)
        return TRUE;
    return FALSE;
}
