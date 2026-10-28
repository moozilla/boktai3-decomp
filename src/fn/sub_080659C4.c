#include "actor_attachment_state.h"
void ActorAttachment_UpdateValueFrame(struct ActorAttachmentSlotView *);
u8 ActorAttachment_TickRemoval(void *, void *);
void ActorAttachment_TickValueFrame(struct ActorAttachmentSlotView *slot)
{
    struct ActorAttachmentManagerView *manager = gActorAttachmentManager;
    if (manager != NULL) {
        struct ActorAttachmentManagerView *owner = manager;
        struct ContactSwarmSprite *sprite = &slot->sprite;
        sprite->flags &= -2;
        ActorAttachment_UpdateValueFrame(slot);
        sprite->frame = slot->frame;
        if (slot->anchor != NULL) {
            s32 dy;
            u32 one;
            struct ContactPosition *position = slot->anchor;
            sprite->position = *position;
            dy = slot->position.pad;
            one = 1;
            if (!ActorAttachment_TestByteFlags(&slot->status[0], one)) {
                s32 wave = gUnk_086149C4[slot->phase[0]];
                dy += wave >> 8;
            }
            sprite->position.x += slot->offset.x;
            sprite->position.y = slot->offset.y + dy + sprite->position.y;
            sprite->position.z += slot->offset.z;
        }
        if (!ActorAttachment_TickRemoval(owner, slot))
            slot->phase[0] += 4;
    }
}
