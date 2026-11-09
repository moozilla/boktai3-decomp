#include "actor_attachment_rising_playback.h"
#include "engine.h"
#include "engine_globals.h"
#include "ram.h"

void ActorAttachment_TickRisingPart(struct ActorAttachmentSlotView *slot)
{
    if (gActorAttachmentManager) {
        struct ActorAttachmentManagerView *manager = gActorAttachmentManager;
        struct ActorAttachmentPartSprite *sprite = slot->parts;
        struct DisplaySequenceState *sequence = slot->sequences;
        const struct ActorAttachmentPartResource *resource;
        switch (slot->kind) {
            case 2:
                resource = manager->partResource;
                break;
            case 3:
                resource = manager->partResource;
                break;
            default:
                resource = NULL;
                break;
        }
        ActorAttachment_StepRisingPart(sprite, sequence, resource);
        slot->jitter[0].x = ((slot->counters[0] >> 1) * gUnk_086149C4[slot->phase[0]]) >> 12;
        slot->jitter[0].y += 2;
        {
            s32 sample;
            gRandomIndex = (gRandomIndex + 1) & 0x3FF;
            sample = *(u16 *)(RANDOM_TABLE_ADDR + gRandomIndex * 2);
            slot->phase[0] += (sample >> 4) & 7;
        }
        sprite->position.x = slot->anchor->x + slot->jitter[0].x;
        sprite->position.y = slot->anchor->y + slot->jitter[0].y + slot->position.z;
        sprite->position.z = slot->anchor->z - slot->jitter[0].x;
        sprite->flags &= ~1;
        if (slot->jitter[0].y <= 60)
            sequence->index = 0;
        else if (slot->jitter[0].y <= 120)
            sequence->index = 1;
        else
            sequence->index = 2;
        if (slot->jitter[0].y > 210) {
            s32 i;
            slot->flags31 = 0;
            {
                s32 id = slot->slotId;
                manager->occupiedMask &= ~(1 << id);
            }
            sprite->flags |= 1;
            for (i = 0; i <= 2; i++) {
                slot->status[i] = 0;
                slot->phase[i] = 0;
                slot->counters[i] = 0;
                slot->thresholds[i] = 0;
            }
            slot->update = NULL;
            Effect_Remove(sprite);
            slot->partMask = 0;
            slot->slotId = 0xFF;
        } else {
            slot->counters[0]++;
        }
    }
}
