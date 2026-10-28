#include "actor_attachment_manager.h"
void Effect_Remove(void *);
static inline u8 Occupied(struct ActorAttachmentManagerView *manager, s32 i)
{
    u32 *mask = &manager->occupiedMask;
    u32 bit;
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
            s32 j = 0;
            struct ActorAttachmentPartSprite *part;
            part = (*(&slot))->parts;
            do {
                s32 bits = slot->partMask >> j;
                s32 bit = 1;
                if (bits & bit) {
                    Effect_Remove(part);
                }
                part++;
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
