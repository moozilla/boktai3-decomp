#include "actor_attachment_manager.h"
void Effect_Remove(void *);
struct ActorAttachmentSlotView *
ActorAttachment_ReleaseResource(struct ActorAttachmentSlotView *slot, u32 resourceKind, void *update)
{
    struct ActorAttachmentManagerView *manager;
    struct ActorAttachmentPartSprite *parts;
    struct ContactSwarmSprite *sprite;
    if (!slot || !(manager = gActorAttachmentManager) || slot->slotId < 0 || slot->update != update)
        return NULL;
    parts = slot->parts;
    sprite = &slot->sprite;
    if (slot->resourceKind == resourceKind) {
        u32 kind;
        {
            s32 id = slot->slotId;
            manager->occupiedMask &= ~(1 << id);
        }
        kind = slot->resourceKind;
        if (kind <= 1 || kind == 4 || kind == 5 || kind == 6) {
            if (slot->active48) {
                Sprite_Remove(sprite);
                slot->active48 = 0;
            }
        } else {
            s32 i;
            for (i = 0; i <= 2; i++) {
                if ((slot->partMask >> i) & 1)
                    Effect_Remove(&parts[i]);
            }
            slot->partMask = 0;
        }
        Memory_Clear(slot, 0x138);
        slot->slotId = 0xFF;
        return NULL;
    }
    return slot;
}
