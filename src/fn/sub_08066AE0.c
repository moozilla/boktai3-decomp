// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_manager.h"
#define ActorAttachment_TickManager sub_08066AE0
extern void *gUnk_08605B34[];
s32 ActorAttachment_TickManager(struct ActorAttachmentManagerView *incoming)
{
    register struct ActorAttachmentManagerView *manager asm("r1") = incoming;
    register struct ActorAttachmentSlotView *base asm("r2") = manager->slots;
    if (manager->occupiedMask) {
        void **dispatch = gUnk_08605B34;
        u8 *active = &manager->slots[0].active;
        struct ActorAttachmentSlotView *slot = base;
        s32 i = 7;
        do {
            sub_0824923C(slot, dispatch[*active]);
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
