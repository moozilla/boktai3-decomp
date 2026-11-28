#include "global.h"
#include "engine/video/sprite_drawable_api.h"

// Re-queues an idle particle; flag 0x80 picks the second list.
void sub_08217E40(struct SpriteDrawable *sprite, u32 flags)
{
    if (!sprite->linked) {
        u32 secondList = flags & 0x80;
        Effect_LinkToList(sprite, secondList != 0);
    }
}
