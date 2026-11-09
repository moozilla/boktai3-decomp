#include "actor_attachment_state.h"
void ActorAttachment_TickWideShake(void *, struct ActorAttachmentSlotView *);
void ActorAttachment_TickLandingBounce(struct ActorAttachmentSlotView *slot)
{
    struct ActorAttachmentManagerView *manager = gActorAttachmentManager;
    if (manager) {
        struct ContactSwarmSprite *sprite = &slot->sprite;
        sprite->flags &= ~1;
        sprite->frame = slot->frame;
        if (slot->anchor) {
            sprite->position = *slot->anchor;
            if (slot->counters[0] <= 3) {
                slot->unknown34 += 20;
                slot->position.pad = slot->unknown34 + (slot->anchor->y + slot->position.z);
                sprite->position.y = slot->position.pad;
            } else if (slot->counters[0] <= 9) {
                slot->timer = 1;
                sprite->position.y = slot->unknown34 + (slot->anchor->y + slot->position.z);
                slot->status[0] |= 1;
                ActorAttachment_TickWideShake(manager, slot);
            } else {
                s32 y = slot->anchor->y + slot->position.z + (s16)slot->unknown34;
                s32 wave = gUnk_086149C4[slot->phase[0]];
                sprite->position.y = y + (wave >> 8);
                slot->phase[0] += 4;
            }
            sprite->position.x += slot->offset.x;
            sprite->position.y += slot->offset.y;
            sprite->position.z += slot->offset.z;
            if (slot->counters[0] <= 9)
                slot->counters[0]++;
        }
    }
}
