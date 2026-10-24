#include "global.h"
#include "contact_actor.h"
s32 Math_Atan2(s32, s32);

// confidence=low: MotionPool_AdjustHeading for another projectile layout: turns toward the
// tracked target by at most turnSpeed while the tracking frames last.
struct TrackingParams {
    u8 unknown0[4];
    u16 trackingFrames;
    u8 direction;
    u8 turnSpeed;
    u8 unknown8[0x38 - 8];
    const struct ContactPosition *target;
};
struct TrackingProjectile {
    u32 frame;
    u32 flags;
    u8 unknown8[0x28 - 8];
    struct TrackingParams params;
    u8 unknown64[0xb8 - 0x64];
    struct ContactPosition position;
};
#define Projectile_AdjustHeading sub_081FD4C0
void Projectile_AdjustHeading(struct TrackingProjectile *slot)
{
    struct TrackingParams *params = &slot->params;
    if (slot->frame < params->trackingFrames) {
        const struct ContactPosition *target = params->target;
        if (target) {
            struct ContactPosition *position = &slot->position;
            u32 byte = (u8)Math_Atan2(target->x - position->x, target->z - position->z);
            u32 desired;
            u32 current;
            s32 delta;
            u32 step;
            s32 limit;
            u32 flags;
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
