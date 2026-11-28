#include "scene/actor/actor_attachment_manager.h"
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
            s32 j;

            for (j = 0; j <= 2; j++) {
                if ((slot->partMask >> j) & 1)
                    Effect_Remove(&slot->parts[j]);
            }
            if (slot->active48)
                Sprite_Remove(&slot->sprite);
        }
    }
    gActorAttachmentManager = NULL;
    return 0;
}
