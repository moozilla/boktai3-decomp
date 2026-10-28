// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_manager.h"
void Effect_Remove(void *);
struct ActorAttachmentSlotView *
ActorAttachment_ReleasePartsResource(struct ActorAttachmentSlotView *incoming, u32 resourceKind,
                                     void *update)
{
    register struct ActorAttachmentSlotView *slot asm("r6") = incoming;
    struct ActorAttachmentManagerView *manager;
    void *matchUpdate;
    struct ActorAttachmentPartSprite *parts;
    s8 *slotId;
    u32 matchKind = resourceKind;
    asm("" : "+r"(matchKind), "+r"(update));
    matchUpdate = update;
    asm("" : "+r"(matchUpdate));
    if (!slot)
        goto cleared;
    manager = gActorAttachmentManager;
    if (!manager)
        goto cleared;
    parts = slot->parts;
    {
        s8 *base = &slot->slotId;
        s32 index = *base;
        slotId = base;
        if (index < 0)
            goto cleared;
    }
    if (slot->update != matchUpdate)
        goto cleared;
    if (slot->resourceKind != matchKind)
        goto unchanged;
    {
        register s8 *low asm("r1") = slotId;
        s32 index = *low;
        s32 i;
        u8 *mask;
        struct ActorAttachmentPartSprite *part;
        manager->occupiedMask &= ~(1 << index);
        i = 0;
        mask = &slot->partMask;
        part = parts;
        do {
            s32 bits = *mask >> i;
            register s32 bit asm("r1") = 1;
            if (bits & bit)
                Effect_Remove(part);
            part++;
            i++;
        } while (i <= 2);
        *mask = 0;
    }
    Memory_Clear(slot, 0x138);
    {
        u32 invalid = 255;
        *(u8 *)slotId = invalid;
    }
cleared:
    return 0;
unchanged:
    return slot;
}
