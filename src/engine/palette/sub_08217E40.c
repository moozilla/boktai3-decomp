#include "engine/shared/global.h"
#include "engine/shared/sprite_drawable_api.h"

// Re-queues an idle particle; flag 0x80 picks the second list.
void sub_08217E40(struct SpriteDrawable *sprite, u32 flags)
{
    u32 queued = sprite->linked;
    if (!queued)
        Effect_LinkToList(sprite, (u32)(-(flags & 0x80)) >> 31);
}
