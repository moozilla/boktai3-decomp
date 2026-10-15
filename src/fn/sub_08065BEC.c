#include "actor_attachment_state.h"
#define ActorAttachment_AllocateSlot sub_08065BEC
static inline u8 ActorAttachment_TestOccupied(u32 *mask, s32 index)
{
    if (*mask & (1 << index))
        return TRUE;
    return FALSE;
}
struct ActorAttachmentSlotView *
ActorAttachment_AllocateSlot(struct ActorAttachmentManagerView *manager)
{
    s32 index;
    struct ActorAttachmentSlotView *slot;
    for (index = 0; index < 8; index++) {
        if (!ActorAttachment_TestOccupied(&manager->occupiedMask, index)) {
            slot = &manager->slots[index];
            manager->occupiedMask |= 1 << index;
            slot->slotId = index;
            return slot;
        }
    }
    return NULL;
}
