#define MotionPool_FinishOnResourceContact sub_080454C0
#include "pooled_motion.h"
void MotionPool_FinishOnResourceContact(void *a, void *b, struct MotionPoolSlot *slot)
{
    slot->frame = slot->params.endFrame;
}
