// MATCHING_CONCESSION: register-pinning
#include "pooled_motion.h"
#define MotionPool_AdjustHeading sub_08045984
void MotionPool_AdjustHeading(struct MotionPoolSlot *slot)
{
    struct MotionPoolParams *params = &slot->params;
    if (slot->frame < params->trackingFrames) {
        const struct ContactPosition *target = slot->target;
        if (target) {
            struct ContactPosition *position = &slot->sprite.position;
            u32 byte = (u8)Math_Atan2(target->x - position->x, target->z - position->z);
            register u32 desired asm("r6");
            u32 current;
            s32 delta;
            u32 step;
            s32 limit;
            u32 flags;
            asm("" : "+r"(byte));
            desired = byte;
            current = (u8)params->direction;
            delta = (s8)desired;
            delta = (s8)(delta - current);
            step = params->turnSpeed;
            limit = (s8)params->turnSpeed;
            if (delta > limit) {
                delta = current + step;
                params->direction = delta;
            } else if (delta < limit) {
                delta = current - step;
                params->direction = delta;
            } else
                params->direction = desired;
            {
                u32 flag = 1;
                flags = slot->flags;
                slot->flags = flags | flag;
            }
        }
    }
}
