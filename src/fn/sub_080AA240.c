#include "global.h"
#include "scene_actor_reset.h"
extern const s16 gUnk_086149C4[];
struct TentacleWaveSegment {
    u8 unknown0[0x1C];
    struct ContactPosition position;
    u8 unknown24[0x105 - 0x24];
    u8 flags;
    u8 unknown106[3];
    u8 phase;
    u8 unknown10A[0x128 - 0x10A];
};
struct TentacleWaveTrail {
    u8 unknown0[0x3D4];
    struct TentacleWaveSegment segments[4];
    u8 unknown874[0x8D2 - 0x874];
    u8 phase;
    u8 unknown8D3[7];
    s16 count;
};
struct TentacleWaveDrawable {
    u8 unknown0[0x1C];
    struct ContactPosition position;
};
static inline s32 Tentacle_TruncateFixed12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline u8 Tentacle_TestWaveFlags(u8 *flags, u32 mask)
{
    if (*flags & mask)
        return 1;
    return 0;
}
static inline void Tentacle_MaskWaveFlags(u8 *flags, u32 mask)
{
    *flags = mask & *flags;
}

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
void sub_080AA240(struct SceneActorRegistryResetView *actor)
{
    struct TentacleWaveTrail *trail = actor->context;
    struct ContactPosition output, previous;
    struct TentacleWaveDrawable *xHost, *zHost;
    s32 sign, i;
    xHost = actor->drawable;
    xHost->position.x += Tentacle_TruncateFixed12(gUnk_086149C4[(trail->phase + 64) & 255] * 2);
    zHost = actor->drawable;
    zHost->position.z += Tentacle_TruncateFixed12(gUnk_086149C4[trail->phase] * 16);
    trail->phase += 8;
    sign = 1;
    for (i = 1; i < trail->count - 1; i += 2) {
        trail->segments[i].position.x +=
            sign * Tentacle_TruncateFixed12(gUnk_086149C4[trail->segments[i].phase] * 4);
        trail->segments[i].phase += 8;
        sign = -sign;
    }
    previous = ((struct TentacleWaveDrawable *)actor->drawable)->position;
    for (i = 0; i < trail->count - 1; i += 2) {
        Tentacle_InterpolatePosition(&output, &previous, &trail->segments[i + 1].position, 1, 512,
                                     0, 0);
        trail->segments[i].position = output;
        previous = trail->segments[i + 1].position;
    }
}

void sub_080AA4B8(struct SceneActorRegistryResetView *actor)
{
    struct TentacleWaveTrail *trail = actor->context;
    s32 live = 0;
    s32 i;
    for (i = 0; i < trail->count; i++) {
        if (!Tentacle_TestWaveFlags(&trail->segments[i].flags, 1)) {
            if (live == 1)
                live = 0;
            else {
                Tentacle_MaskWaveFlags(&trail->segments[i].flags, ~2);
                live++;
            }
        }
    }
}
