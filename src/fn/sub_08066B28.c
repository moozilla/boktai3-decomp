// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_manager.h"
#define ActorAttachment_DestroyManager sub_08066B28
void Effect_Remove(void *);
static inline u8 Occupied(struct ActorAttachmentManagerView *manager, s32 i)
{
    register u32 *mask asm("r0") = &manager->occupiedMask;
    register u32 bit asm("r1");
    bit = 1;
    if (*mask & (bit << i))
        return TRUE;
    return FALSE;
}
s32 ActorAttachment_DestroyManager(struct ActorAttachmentManagerView *manager)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (Occupied(manager, i)) {
            struct ActorAttachmentSlotView *slot = &manager->slots[i];
            register s32 j asm("r6") = 0;
            struct ActorAttachmentPartSprite *part;
            part = slot->parts;
            do {
                s32 bits = slot->partMask >> j;
                register s32 bit asm("r1") = 1;
                if (bits & bit)
                    Effect_Remove(part);
                part++;
                j++;
            } while (j <= 2);
            if (slot->unknown48[0])
                Sprite_Remove(&slot->sprite);
        }
    }
    {
        register struct ActorAttachmentManagerView *empty asm("r1") = 0;
        gUnk_02000124 = empty;
    }
    return 0;
}
