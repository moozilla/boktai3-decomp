#include "nav_steering.h"

// 8-direction variant of sub_0821FB8C: steers a nav probe; when the probe lies to the side of `angle`
// (relative to `target`), or `target` cannot move along `angle`, snaps to an 8-way direction and sets flag 2.
s32 sub_0821FC34(struct NavSteeringProbe *probe, struct NavSteeringPosition *target, s32 angle,
                 s32 dist)
{
    struct NavSteeringPosition d;
    s32 a, diff, ret;

    NavMesh_UpdateProbe(probe, target);
    probe->flags &= ~2;
    NavSteering_Subtract(&d, &probe->pos, target);
    a = Math_Atan2(d.x, d.z);
    diff = (a - angle) & 0xFF;
    if ((u32)(diff - 0x18) <= 0xD0) {
        ret = a + 0x10;
        ret &= 0xE0;
        probe->flags |= 2;
        return ret;
    }
    if (Actor_MoveAlongAngle(target, angle, dist) == 0) {
        if (diff <= 0x1F)
            a += 0x20;
        a &= 0xE0;
        NAV_STEERING_MARK_BLOCKED(probe);
        return a;
    }
    return angle;
}
