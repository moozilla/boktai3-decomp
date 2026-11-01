#ifndef GUARD_ARMOR_DECISION_OPS_H
#define GUARD_ARMOR_DECISION_OPS_H

/* Coordinate and state operations shared by Armor attack decisions. */
/* The signed component is retained alongside its initialized magnitude. */
#define ARMOR_ABSOLUTE_COMPONENT(magnitude, value)                                                 \
    do {                                                                                           \
        if ((value) < 0)                                                                           \
            (magnitude) = -(value);                                                                \
    } while (0)
#define ARMOR_CLEAR_DECISION(field)                                                                \
    do {                                                                                           \
        *(field) = 0;                                                                              \
    } while (0)
#define ARMOR_BRANCH_OUTSIDE_GRID(magnitude, maximum, label)                                       \
    do {                                                                                           \
        if ((magnitude) > (maximum))                                                               \
            goto label;                                                                            \
    } while (0)

#define ARMOR_ROUND_GRID_COMPONENT(value)                                                          \
    do {                                                                                           \
        (value) = ((value) + 128) >> 8;                                                            \
    } while (0)
#define ARMOR_BRANCH_FACING_AXIS(actor, delta, negative, positive, aligned, unaligned)             \
    do {                                                                                           \
        if ((delta) < 0) {                                                                         \
            if ((actor)->facing == (negative))                                                     \
                goto aligned;                                                                      \
        } else if ((actor)->facing == (positive))                                                  \
            goto aligned;                                                                          \
        goto unaligned;                                                                            \
    } while (0)
#define ARMOR_CAPTURE_COMPONENT_DELTA(out, target, actor)                                          \
    do {                                                                                           \
        (out) = (target) - (actor);                                                                \
    } while (0)
#define ARMOR_BRANCH_AXIS_UNALIGNED(x, z, label)                                                   \
    do {                                                                                           \
        if ((x) != 0 && (z) != 0)                                                                  \
            goto label;                                                                            \
    } while (0)

/* Grid-cell deltas from actor to target: x and z rounded to 256-unit cells, with each
 * component's magnitude (y is left unrounded). */
#define ARMOR_ALIGNED_GRID(deltaX, deltaY, deltaZ, absX, absY, absZ, target, actor)                \
    do {                                                                                           \
        (deltaX) = (target)->x - (actor)->x;                                                       \
        (deltaY) = (target)->y - (actor)->y;                                                       \
        (deltaZ) = (target)->z - (actor)->z;                                                       \
        (deltaX) = ((deltaX) + 128) >> 8;                                                          \
        (deltaZ) = ((deltaZ) + 128) >> 8;                                                          \
        (absX) = (deltaX);                                                                         \
        (absY) = (deltaY);                                                                         \
        (absZ) = (deltaZ);                                                                         \
        if ((deltaX) < 0)                                                                          \
            (absX) = -(deltaX);                                                                    \
        if ((deltaY) < 0)                                                                          \
            (absY) = -(deltaY);                                                                    \
        if ((deltaZ) < 0)                                                                          \
            (absZ) = -(deltaZ);                                                                    \
    } while (0)
/* The x/z form, with each position component evaluated once. */
#define ARMOR_GET_RELATIVE_GRID(deltaX, deltaZ, absX, absZ, targetX, actorX, targetZ, actorZ)      \
    do {                                                                                           \
        (deltaX) = (targetX) - (actorX);                                                           \
        (deltaZ) = (targetZ) - (actorZ);                                                           \
        (deltaX) = ((deltaX) + 0x80) >> 8;                                                         \
        (deltaZ) = ((deltaZ) + 0x80) >> 8;                                                         \
        (absX) = (deltaX);                                                                         \
        (absZ) = (deltaZ);                                                                         \
        if ((deltaX) < 0)                                                                          \
            (absX) = -(deltaX);                                                                    \
        if ((deltaZ) < 0)                                                                          \
            (absZ) = -(deltaZ);                                                                    \
    } while (0)

#endif
