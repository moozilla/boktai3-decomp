#include "global.h"
#include "level_advance_display.h"

s32 Effect_LinkToList(struct LevelAdvanceParticle *, u32);

// Re-queues an idle particle; flag 0x80 picks the second list.
void sub_08217E40(struct LevelAdvanceParticle *sprite, u32 flags)
{
    u32 queued = sprite->queued;
    if (!queued)
        Effect_LinkToList(sprite, (u32)(-(flags & 0x80)) >> 31);
}
