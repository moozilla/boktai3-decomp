#include "actor_attachment_manager.h"
void Effect_Remove(void *);
static inline u8 Occupied(struct ActorAttachmentManagerView *manager, s32 i)
{
    u32 *mask = &manager->occupiedMask;
    if (*mask & (1 << i))
        return TRUE;
    return FALSE;
}
s32 ActorAttachment_DestroyManager(struct ActorAttachmentManagerView *manager)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (Occupied(manager, i)) {
            struct ActorAttachmentSlotView *slot = &manager->slots[i];
            s32 j = 0;
            do {
                s32 bits = slot->partMask >> j;
                if (bits & 1) {
                    Effect_Remove(&slot->parts[j]);
                }
                j++;
            } while (j <= 2);
            if (slot->active48) {
                Sprite_Remove(&slot->sprite);
            }
        }
    }
    {
        struct ActorAttachmentManagerView *empty = 0;
        gActorAttachmentManager = empty;
    }
    return 0;
}
