#ifndef GUARD_MOVEMENT_CURVE_H
#define GUARD_MOVEMENT_CURVE_H

#include "contact_position.h"
#include "engine_globals.h"

static inline s32 MovementCurve_TruncateFixed10(s32 value)
{
    return value >= 0 ? value >> 10 : -((-value) >> 10);
}
static inline void MovementCurve_SubtractPositions(struct ContactPosition *out,
                                                   struct ContactPosition *a,
                                                   struct ContactPosition *b)
{
    out->x = b->x - a->x;
    out->y = b->y - a->y;
    out->z = b->z - a->z;
}
static inline void MovementCurve_InterpolatePosition(struct ContactPosition *out,
                                                     struct ContactPosition *start,
                                                     struct ContactPosition *end, s32 initialScale,
                                                     s32 t, s32 finalScale, s32 speed)
{
    struct ContactPosition delta, tangent;
    s32 square, cube, twiceCube, thriceSquare, sway, final;
    s32 scaled, value;
    MovementCurve_SubtractPositions(&delta, start, end);
    ZeroFill8(&tangent);
    square = MovementCurve_TruncateFixed10(t * t);
    cube = MovementCurve_TruncateFixed10(square * t);
    twiceCube = cube * 2;
    thriceSquare = square * 3;
    sway = (cube - 2 * square + t) * speed;
    final = (cube - square) * finalScale;
    scaled = ((twiceCube - thriceSquare + 1) * tangent.x) * initialScale;
    value = (thriceSquare - twiceCube) * delta.x;
    value = scaled + value + sway + final;
    out->x = start->x + MovementCurve_TruncateFixed10(value);
    scaled = ((twiceCube - thriceSquare + 1) * tangent.y) * initialScale;
    value = (-twiceCube + thriceSquare) * delta.y;
    value = scaled + value + sway + final;
    out->y = start->y + MovementCurve_TruncateFixed10(value);
    scaled = ((twiceCube - thriceSquare + 1) * tangent.z) * initialScale;
    value = (-twiceCube + thriceSquare) * delta.z;
    value = scaled + value + sway + final;
    out->z = start->z + MovementCurve_TruncateFixed10(value);
}
static inline u32 MovementCurve_RandomByte(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)(RANDOM_TABLE_ADDR + index) & 255;
}

#endif
