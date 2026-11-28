#include "scene/actor/actor_attachment_manager.h"
extern void *gUnk_08605B34[];
s32 ActorAttachment_TickManager(struct ActorAttachmentManagerView *manager)
{
    struct ActorAttachmentSlotView *slots = manager->slots;

    if (manager->occupiedMask) {
        s32 i;

        for (i = 0; i < 8; i++)
            Callback_InvokeActor(&slots[i], gUnk_08605B34[slots[i].flags31]);
    }
    return 0;
}
