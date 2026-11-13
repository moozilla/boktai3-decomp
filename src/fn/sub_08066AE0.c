#include "actor_attachment_manager.h"
extern void *gUnk_08605B34[];
s32 ActorAttachment_TickManager(struct ActorAttachmentManagerView *incoming)
{
    struct ActorAttachmentManagerView *manager = incoming;
    struct ActorAttachmentSlotView *base = manager->slots;
    if (manager->occupiedMask) {
        void **dispatch = gUnk_08605B34;
        u8 *active = &manager->slots[0].flags31;
        struct ActorAttachmentSlotView *slot = base;
        s32 i = 7;
    next_slot: {
        Callback_InvokeActor(slot, dispatch[*active]);
        active += sizeof(*slot);
        slot++;
        i--;
        if (i >= 0)
            goto next_slot;
    }
    }
    return 0;
}
