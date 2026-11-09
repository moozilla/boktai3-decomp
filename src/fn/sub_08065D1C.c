#include "actor_attachment_manager.h"
void Effect_Remove(void *);
struct ActorAttachmentSlotView *
ActorAttachment_ReleasePartsResource(struct ActorAttachmentSlotView *slot, u32 resourceKind, void *update)
{
    struct ActorAttachmentManagerView *manager;
    struct ActorAttachmentPartSprite *parts;
    if (!slot || !(manager = gActorAttachmentManager))
        return NULL;
    parts = slot->parts;
    if (slot->slotId < 0 || slot->update != update)
        return NULL;
    if (slot->resourceKind == resourceKind) {
        s32 i;
        {
            s32 id = slot->slotId;
            manager->occupiedMask &= ~(1 << id);
        }
        for (i = 0; i <= 2; i++) {
            if ((slot->partMask >> i) & 1)
                Effect_Remove(&parts[i]);
        }
        slot->partMask = 0;
        Memory_Clear(slot, 0x138);
        slot->slotId = 0xFF;
        return NULL;
    }
    return slot;
}
