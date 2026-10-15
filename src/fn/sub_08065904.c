// MATCHING_CONCESSION: register-pinning
#include "actor_attachment_state.h"
void ActorAttachment_UpdateValueFrame(struct ActorAttachmentSlotView *input)
{
    struct ActorAttachmentSlotView *slot = input;
    u32 four = 4;
    u32 test = ActorAttachment_TestByteFlags(&slot->status[0], four);
    u32 finished;
    finished = test;
    asm("" : : "r"(finished));
    if (!finished) {
        struct ActorAttachmentSlotView *view = slot;
        register s16 *value asm("r0");
        s32 current;
        s16 *sample;
        asm("" : : "r"(view));
        value = view->value;
        current = *value;
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
            register s32 level asm("r3") = 4;
            s32 currentValue = *sample;
            register s16 *threshold asm("r1");
            s32 boundary;
            register s16 *cutoff asm("r5");
            u8 *status;
            s16 *timer;
            register u8 *flags asm("r8");
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
            register s16 *timerView asm("r5");
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
