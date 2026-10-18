// MATCHING_CONCESSION: register-pinning
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
        do {
            Callback_InvokeActor(slot, dispatch[*active]);
            /* Integer address advances preserve the two parallel field walks. */
            {
                register u32 stride asm("r0") = sizeof(*slot);
                active = (u8 *)((u32)active + stride);
                slot = (struct ActorAttachmentSlotView *)((u32)slot + stride);
            }
            i--;
        } while (i >= 0);
    }
    return 0;
}
