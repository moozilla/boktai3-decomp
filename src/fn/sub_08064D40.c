// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_state.h"
void ActorAttachment_BeginPartJitter(struct ActorAttachmentSlotView *slot, s32 indexInput)
{
    s32 index = indexInput;
    if (gActorAttachmentManager != NULL) {
        if (*slot->value <= slot->thresholds[index]) {
            u8 *status;
            s32 indexLow;
            {
                u8 *statusBase = slot->status;
                indexLow = index;
                status = statusBase + indexLow;
            }
            if (*status != 1) {
                register const u16 *table asm("r5") = gRandomTable;
                u32 *randomIndex = &gRandomIndex;
                u32 mask;
                register u32 random asm("r1");
                struct ActorAttachmentSlotView *part;
                u32 smallMask;
                s32 x;
                u32 zero;
                u32 sample;
                random = *randomIndex + 1;
                mask = 0x3FF;
                random &= mask;
                sample = table[random];
                part = (void *)((u32)slot + indexLow * 8);
                smallMask = 15;
                x = (sample & smallMask) - 8;
                zero = 0;
                part->jitter[0].x = x;
                random = (random + 1) & mask;
                sample = table[random];
                sample &= smallMask;
                part->jitter[0].y = sample;
                random = (random + 1) & mask;
                *randomIndex = random;
                sample = table[random];
                sample &= smallMask;
                sample -= 8;
                part->jitter[0].z = sample;
                {
                    register u32 flags asm("r1") = *status;
                    u32 updated;
                    updated = 1 | flags;
                    *status = updated;
                }
                slot->counters[index] = zero;
            }
        }
    }
}
