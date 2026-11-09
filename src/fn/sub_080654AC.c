#include "actor_attachment_sprite_playback.h"
#include "engine.h"

void ActorAttachment_TickWideShake(void *, struct ActorAttachmentSlotView *);
void ActorAttachment_TickDescendingSprite(struct ActorAttachmentSlotView *slot)
{
    if (gActorAttachmentManager) {
        struct ActorAttachmentManagerView *manager = gActorAttachmentManager;
        struct ContactSwarmSprite *sprite = &slot->sprite;
        struct DisplaySequenceState *sequence = slot->sequences;
        u32 ended = ActorAttachment_StepSprite(sprite, sequence);
        if (slot->frame <= 1) {
            slot->unknown34 += 25;
            slot->position.pad = slot->unknown34 + (slot->anchor->y + slot->position.z);
        } else {
            slot->unknown34 += 10;
            slot->position.pad = slot->unknown34 + (slot->anchor->y + slot->position.z);
        }
        if (slot->position.pad > slot->position.y + slot->anchor->y)
            slot->position.pad = slot->position.y + slot->anchor->y;
        if (slot->position.pad < slot->position.y + slot->anchor->y || slot->timer > 0) {
            sequence->index = 0;
            sequence->elapsed = 0;
        }
        if (slot->counters[0] <= 29) {
            sequence->index = 0;
            sequence->elapsed = 0;
        }
        if (ended) {
            slot->flags31 = 0;
            {
                s32 id = slot->slotId;
                manager->occupiedMask &= ~(1 << id);
            }
            sprite->flags |= 1;
            slot->status[0] = 0;
            slot->update = NULL;
            slot->counters[0] = 0;
            Sprite_Remove(sprite);
            slot->active48 = 0;
            slot->slotId = -1;
        } else {
            sprite->flags &= ~1;
            if (slot->anchor) {
                sprite->position = *slot->anchor;
                sprite->position.x += slot->offset.x;
                sprite->position.y = slot->position.pad + slot->offset.y;
                sprite->position.z += slot->offset.z;
            }
            ActorAttachment_TickWideShake(manager, slot);
            slot->counters[0]++;
        }
    }
}
