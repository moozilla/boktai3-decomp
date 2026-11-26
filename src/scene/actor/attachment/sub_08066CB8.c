#include "scene/actor/actor_attachment_manager.h"
/* ActorAttachment_ReleaseAll: Releases occupied attachment slots and removes their sprites or parts without identity filtering */
#define ActorAttachment_ReleaseAll sub_08066CB8
void Effect_Remove(void *);
static inline u8 Occupied(u32 *mask, s32 i)
{
    if (*mask & (1 << i))
        return TRUE;
    return FALSE;
}
void ActorAttachment_ReleaseAll(void)
{
    struct ActorAttachmentManagerView *manager;
    s32 i;
    if (!gActorAttachmentManager)
        return;
    manager = gActorAttachmentManager;
    if (!manager->occupiedMask)
        return;
    for (i = 0; i < 8; i++) {
        if (Occupied(&manager->occupiedMask, i)) {
            struct ActorAttachmentSlotView *slot = &manager->slots[i];
            struct ActorAttachmentPartSprite *parts = slot->parts;
            struct ContactSwarmSprite *sprite = &slot->sprite;
            if (slot->slotId >= 0) {
                u32 kind;
                {
                    s8 id = slot->slotId;
                    manager->occupiedMask &= ~(1 << id);
                }
                kind = slot->resourceKind;
                if (kind <= 1 || kind == 4 || kind == 5 || kind == 6) {
                    if (slot->active48) {
                        Sprite_Remove(sprite);
                        slot->active48 = 0;
                    }
                } else {
                    s32 j;
                    for (j = 0; j <= 2; j++) {
                        if ((slot->partMask >> j) & 1)
                            Effect_Remove(&parts[j]);
                    }
                    slot->partMask = 0;
                }
                Memory_Clear(slot, 0x138);
                slot->slotId = 0xFF;
            }
        }
    }
}
