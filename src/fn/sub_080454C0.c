#include "pooled_motion.h"
void MotionPool_FinishOnResourceContact(void *a, void *b, struct MotionPoolSlot *slot)
{
    slot->frame = slot->params.endFrame;
}
