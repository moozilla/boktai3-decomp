// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_manager.h"
void Effect_Remove(void *);
struct ActorAttachmentSlotView *
ActorAttachment_ReleaseResource(struct ActorAttachmentSlotView *incoming, u32 resourceKind,
                                void *update)
{
    struct ActorAttachmentSlotView *slot = incoming;
    struct ActorAttachmentManagerView *manager;
    void *matchUpdate;
    struct ActorAttachmentPartSprite *parts;
    struct ContactSwarmSprite *sprite;
    u8 *resource;
    s8 *slotId;
    register u32 matchKind asm("r4") = resourceKind;
    asm("" : "+r"(matchKind), "+r"(update));
    matchUpdate = update;
    asm("" : "+r"(matchUpdate));
    asm("" : "+r"(slot));
    if (!slot)
        goto cleared;
    manager = gActorAttachmentManager;
    if (!manager)
        goto cleared;
    {
        s8 *base = &slot->slotId;
        s32 index = *base;
        slotId = base;
        if (index < 0)
            goto cleared;
    }
    if (slot->update != matchUpdate)
        goto cleared;
    parts = slot->parts;
    sprite = &slot->sprite;
    resource = &slot->resourceKind;
    if (*resource != matchKind)
        goto unchanged;
    {
        register s8 *low asm("r1") = slotId;
        s32 index = *low;
        s32 i;
        u8 *mask;
        struct ActorAttachmentPartSprite *part;
        manager->occupiedMask &= ~(1 << index);
        {
            u32 kind = *resource;
            if (kind <= 1 || kind == 4 || kind == 5 || kind == 6) {
                if (slot->active48) {
                    Sprite_Remove(sprite);
                    slot->active48 = 0;
                }
            } else {
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
        }
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
