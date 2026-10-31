#include "global.h"
#include "scene_actor_reset.h"

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

static inline s32 Tentacle_TruncateFixed10(s32 value)
{
    return value >= 0 ? value >> 10 : -((-value) >> 10);
}
static inline void Tentacle_SubtractPositions(struct ContactPosition *out,
                                              struct ContactPosition *a, struct ContactPosition *b)
{
    out->x = b->x - a->x;
    out->y = b->y - a->y;
    out->z = b->z - a->z;
}
static inline void Tentacle_InterpolatePosition(struct ContactPosition *out,
                                                struct ContactPosition *start,
                                                struct ContactPosition *end, s32 initialScale,
                                                s32 t, s32 finalScale, s32 speed)
{
    struct ContactPosition delta, tangent;
    s32 square, cube, twiceCube, thriceSquare, sway, final;
    s32 scaled, value;
    Tentacle_SubtractPositions(&delta, start, end);
    ZeroFill8(&tangent);
    square = Tentacle_TruncateFixed10(t * t);
    cube = Tentacle_TruncateFixed10(square * t);
    twiceCube = cube * 2;
    thriceSquare = square * 3;
    sway = (cube - 2 * square + t) * speed;
    final = (cube - square) * finalScale;
    scaled = ((twiceCube - thriceSquare + 1) * tangent.x) * initialScale;
    value = (thriceSquare - twiceCube) * delta.x;
    value = scaled + value + sway + final;
    out->x = start->x + Tentacle_TruncateFixed10(value);
    scaled = ((twiceCube - thriceSquare + 1) * tangent.y) * initialScale;
    value = (-twiceCube + thriceSquare) * delta.y;
    value = scaled + value + sway + final;
    out->y = start->y + Tentacle_TruncateFixed10(value);
    scaled = ((twiceCube - thriceSquare + 1) * tangent.z) * initialScale;
    value = (-twiceCube + thriceSquare) * delta.z;
    value = scaled + value + sway + final;
    out->z = start->z + Tentacle_TruncateFixed10(value);
}
void sub_080A9BD8(struct SceneActorRegistryResetView *actor, struct ContactPosition *from,
                  struct ContactPosition *to, s32 first, s32 last)
{
    struct TentacleCurveTrail *trail = actor->context;
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
        Tentacle_InterpolatePosition(&output, &start, &end, zero, step * (count + 1), 1, zero);
        trail->segments[index].position = output;
        count++;
    }
}
