#include "sprite_effect_controller.h"
#define SpriteDrawable_QueueIfInactive sub_08217E40
#define SpriteDrawable_InsertQueue     sub_082143CC
s32 sub_082143CC(struct SpriteEffectDrawablePrefix *, u32);
void sub_08217E40(struct SpriteEffectDrawablePrefix *sprite, u32 flags)
{
    u32 queue = flags;
    struct SpriteEffectDrawablePrefix *entry = sprite;
    if (entry->active)
        return;
    sub_082143CC(entry, (u32) - (queue & 0x80) >> 31);
}
