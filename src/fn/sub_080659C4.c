#include "actor_attachment_state.h"
#define ActorAttachment_TickValueFrame sub_080659C4
void sub_08065904(struct ActorAttachmentSlotView *);
u8 sub_0806466C(void *, struct ActorAttachmentSlotView *);
void ActorAttachment_TickValueFrame(struct ActorAttachmentSlotView *slot)
{
    struct ActorAttachmentManagerView *manager = gUnk_02000124;
    if (manager != NULL) {
        struct ActorAttachmentManagerView *owner = manager;
        struct ContactSwarmSprite *sprite = &slot->sprite;
        sprite->flags &= -2;
        sub_08065904(slot);
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
        if (!sub_0806466C(owner, slot))
            slot->phase[0] += 4;
    }
}
