#include "actor_attachment_state.h"
#include "engine_globals.h"

static inline u32 NextRandom(void)
{
    const u16 *table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return table[gRandomIndex];
}

/* A part may begin jittering unless its status is exactly 1. */
static inline u8 ActorAttachment_CanStartPartJitter(struct ActorAttachmentSlotView *slot, s32 index)
{
    return slot->status[index] != 1;
}

void ActorAttachment_BeginPartJitter(struct ActorAttachmentSlotView *slot, s32 index)
{
    if (gActorAttachmentManager != NULL && *slot->value <= slot->thresholds[index] &&
        ActorAttachment_CanStartPartJitter(slot, index)) {
        slot->jitter[index].x = (NextRandom() & 15) - 8;
        slot->jitter[index].y = NextRandom() & 15;
        slot->jitter[index].z = (NextRandom() & 15) - 8;
        slot->status[index] |= 1;
        slot->counters[index] = 0;
    }
}
