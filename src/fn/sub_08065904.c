#include "actor_attachment_state.h"
void ActorAttachment_UpdateValueFrame(struct ActorAttachmentSlotView *input)
{
    struct ActorAttachmentSlotView *slot = input;
    u32 four = 4;
    u32 test = ActorAttachment_TestByteFlags(&slot->status[0], four);
    u32 finished;
    finished = test;
    if (!finished) {
        struct ActorAttachmentSlotView *view = slot;
        s16 *value;
        s16 new_var;
        s32 current;
        s16 *sample;
        value = view->value;
        new_var = *value;
        current = new_var;
        sample = value;
        if (current <= 0) {
            s16 *timer = &slot->timer;
            u8 *status;
            *timer = 20;
            status = slot->status;
            {
                u32 input = *status;
                u32 mask;
                mask = 5;
                mask |= input;
                *status = mask;
            }
            slot->frame = 4;
        } else {
            s32 level = 4;
            s32 currentValue = *sample;
            s16 *threshold;
            s32 boundary;
            s16 *cutoff;
            u8 *status;
            s16 *timer;
            u8 *flags;
            threshold = slot->thresholds;
            boundary = *threshold * 4;
            status = slot->status;
            timer = &slot->timer;
            cutoff = threshold;
            flags = &slot->flags;
            if (currentValue >= boundary)
                slot->frame = finished;
            else {
                do {
                    level--;
                    if (level <= 0)
                        goto reset;
                } while (*sample < *cutoff * level);
                slot->frame = 4 - level;
            }
        reset: {
            s32 zero = 0;
            s16 *timerView;
            timerView = timer;
            *timerView = zero;
            zero -= 6;
            zero &= *status;
            *status = zero;
        }
            {
                u32 input = *flags;
                u32 value;
                value = 0xFE;
                value &= input;
                *flags = value;
            }
        }
    }
}
