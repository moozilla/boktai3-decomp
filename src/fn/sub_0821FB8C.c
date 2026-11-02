#include "nav_steering.h"

// Steers a nav probe: when the probe lies to the side of `angle` (relative to `target`),
// or `target` cannot move along `angle`, snaps to a 4-way direction and sets flag 2.
s32 sub_0821FB8C(struct NavSteeringProbe *probe, struct NavSteeringPosition *target, s32 angle,
                 s32 dist)
{
    struct NavSteeringPosition d;
    s32 a, diff, ret;

    NavMesh_UpdateProbe(probe, target);
    probe->flags &= ~2;
    NavSteering_Subtract(&d, &probe->pos, target);
    a = Math_Atan2(d.x, d.z);
    diff = (a - angle) & 0xFF;
    if ((u32)(diff - 0x30) <= 0xA0) {
        ret = a + 0x20;
        ret &= 0xC0;
        probe->flags |= 2;
        return ret;
    }
    if (Actor_MoveAlongAngle(target, angle, dist) == 0) {
        if (diff <= 0x3F)
            a += 0x40;
        a &= 0xC0;
        NAV_STEERING_MARK_BLOCKED(probe);
        return a;
    }
    return angle;
}
