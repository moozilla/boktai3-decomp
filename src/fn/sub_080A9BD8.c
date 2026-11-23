#include "global.h"
#include "scene_actor_reset.h"
#include "movement_curve.h"

struct TentacleCurveSegment {
    u8 unknown0[0x1C];
    struct ContactPosition position;
    u8 unknown24[0x128 - 0x24];
};
struct TentacleCurveTrail {
    u8 unknown0[0x3D4];
    struct TentacleCurveSegment segments[4];
};
s32 Div(s32, s32);

void sub_080A9BD8(struct ChaseActor *actor, struct ContactPosition *from,
                  struct ContactPosition *to, s32 first, s32 last)
{
    struct TentacleCurveTrail *trail = actor->base.context;
    struct ContactPosition output, start, end;
    s32 step, index, count, zero, before, range;
    start = *from;
    end = *to;
    range = 1024;
    before = first - 1;
    step = Div(range, last - before);
    count = 0;
    zero = 0;
    for (index = first; index < last; index++) {
        MovementCurve_InterpolatePosition(&output, &start, &end, zero, step * (count + 1), 1, zero);
        trail->segments[index].position = output;
        count++;
    }
}
