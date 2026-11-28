#include "scene/actor/actor_attachment_state.h"
void ActorAttachment_UpdateValueFrame(struct ActorAttachmentSlotView *);
u8 ActorAttachment_TickRemoval(void *, void *);
void ActorAttachment_TickValueFrame(struct ActorAttachmentSlotView *slot)
{
    if (gActorAttachmentManager) {
        struct ActorAttachmentManagerView *manager = gActorAttachmentManager;
        struct ContactSwarmSprite *sprite = &slot->sprite;

        sprite->flags &= ~1;
        ActorAttachment_UpdateValueFrame(slot);
        sprite->frame = slot->frame;
        if (slot->anchor != NULL) {
            s32 y;
            u32 shakingBit;

            sprite->position = *slot->anchor;
            y = slot->position.pad;
            shakingBit = 1;
            if (!ActorAttachment_TestByteFlags(&slot->status[0], shakingBit)) {
                s32 wave = gUnk_086149C4[slot->phase[0]];
                y += wave >> 8;
            }
            sprite->position.x += slot->offset.x;
            sprite->position.y += slot->offset.y + y;
            sprite->position.z += slot->offset.z;
        }
        if (!ActorAttachment_TickRemoval(manager, slot))
            slot->phase[0] += 4;
    }
}
