#include "scene/actor/actor_attachment_state.h"
void ActorAttachment_UpdateValueFrame(struct ActorAttachmentSlotView *slot)
{
    u32 depletedBit = 4;
    u32 depleted = ActorAttachment_TestByteFlags(&slot->status[0], depletedBit);

    if (!depleted) {
        s16 *value = slot->value;
        s16 sampled = *value;
        s32 current = sampled;
        s16 *sample = value;

        if (current <= 0) {
            slot->timer = 20;
            slot->status[0] |= 5;
            slot->frame = 4;
        } else {
            s32 level = 4;
            s32 currentValue = *sample;
            s16 *threshold = slot->thresholds;
            s32 boundary = *threshold * 4;
            u8 *status = slot->status;
            s16 *timer = &slot->timer;
            s16 *cutoff = threshold;
            u8 *flags = &slot->flags;

            if (currentValue >= boundary) {
                slot->frame = 0;
            } else {
                do {
                    if (--level <= 0)
                        goto reset;
                } while (*sample < *cutoff * level);
                slot->frame = 4 - level;
            }
        reset: {
            s32 bits = 0;
            *timer = bits;
            bits -= 6;
            *status &= bits;
        }
            *flags &= ~1;
        }
    }
}
