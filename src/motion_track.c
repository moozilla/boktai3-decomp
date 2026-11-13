#include "orbit_motion.h"
#include "motion_track.h"

void MotionTrack_Initialize(struct MotionTrack *track, const void *data,
                            const struct MotionTrackVector *origin,
                            const struct MotionTrackVector *target, u32 mode)
{
    if (data) {
        track->data = data;
        if (origin)
            track->origin = *origin;
        else {
            ZeroFill8(&track->origin);
        }
        if (target)
            track->target = *target;
        else {
            ZeroFill8(&track->target);
        }
        track->mode = mode;
        track->active = 1;
        track->cursor = 0;
    }
}

u16 Sqrt(u32);

s32 __divsi3(s32, s32);

static inline void difference(struct OrbitVector *out, struct OrbitVector *a, struct OrbitVector *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

static inline s32 square(s32 n)
{
    return n * n;
}

static inline s32 OrbitMotion_DistanceXZ(struct OrbitVector *v)
{
    return Sqrt(square(v->x) + square(v->z));
}

void OrbitMotion_BeginDistanceWave(struct OrbitMotion *motion, struct OrbitVector *position,
                                   struct OrbitVector *end, s32 angle, s32 speed)
{
    struct OrbitVector delta;
    if (motion && position && end) {
        struct OrbitVector *target;
        motion->position = position;
        target = &motion->base;
        *target = *end;
        motion->angle = angle;
        difference(&delta, target, motion->position);
        {
            s32 length = OrbitMotion_DistanceXZ(&delta);
            s32 factor = 314 * speed;
            motion->radius = __divsi3(length * factor, 25600);
        }
        if (motion->radius <= 1)
            motion->radius = 2;
        motion->speed = speed;
        motion->mode = ORBIT_MOTION_STEER_XZ;
        motion->state = 1;
        motion->timer = 0;
    }
}

s32 OrbitMotion_CalculateArcCenter(struct OrbitVector *, struct OrbitVector *, struct OrbitVector *,
                                   s32);

s32 Math_Atan2(s32, s32);

void OrbitMotion_BeginArc(struct OrbitMotion *motion, struct OrbitVector *position,
                          struct OrbitVector *end, s32 sweep, s32 duration)
{
    if (motion && position && end) {
        struct OrbitVector *target;
        s32 angle;
        motion->position = position;
        target = &motion->base;
        *target = *end;
        motion->radius = OrbitMotion_CalculateArcCenter(&motion->center, position, target, sweep);
        {
            s32 cx = motion->center.x;
            s32 cz = motion->center.z;
            struct OrbitVector *current = motion->position;
            s32 x = current->x;
            s32 z = current->z;
            angle = Math_Atan2(x - cx, z - cz);
        }
        motion->startAngle = angle;
        motion->endAngle = angle + sweep;
        motion->duration = duration;
        motion->mode = ORBIT_MOTION_ARC;
        motion->state = 1;
        motion->timer = 0;
    }
}

static inline s32 OrbitMotion_DistanceXY(struct OrbitVector *v)
{
    return Sqrt(square(v->x) + square(v->y));
}

void OrbitMotion_BeginVerticalDistanceWave(struct OrbitMotion *motion, struct OrbitVector *position, struct OrbitVector *end,
                  s32 angle, s32 speed)
{
    struct OrbitVector delta;
    if (motion && position && end) {
        struct OrbitVector *target;
        motion->position = position;
        target = &motion->base;
        *target = *end;
        motion->angle = angle;
        difference(&delta, target, motion->position);
        {
            s32 length = OrbitMotion_DistanceXY(&delta);
            s32 factor = 314 * speed;
            motion->radius = __divsi3(length * factor, 25600);
        }
        if (motion->radius <= 1)
            motion->radius = 2;
        motion->speed = speed;
        motion->mode = ORBIT_MOTION_STEER_XY;
        motion->state = 1;
        motion->timer = 0;
    }
}

void OrbitMotion_BeginCrossedSines(struct OrbitMotion *motion, struct OrbitVector *position, s32 radius, s32 radius2,
                  s32 duration)
{
    if (motion && position && duration) {
        motion->position = position;
        motion->base = *position;
        motion->radius = radius;
        motion->radius2 = radius2;
        motion->speed = __divsi3(0x200, duration);
        motion->mode = ORBIT_MOTION_CROSSED_SINES;
        motion->state = 1;
        motion->timer = 0;
    }
}

void PositionMotion_InitParameterizedApproach(struct OrbitMotion *motion, struct OrbitVector *position, struct OrbitVector *target, u32 xAmplitude, u32 yAmplitude,
                                              u32 zAmplitude, u32 angle, u32 speed)
{
    if (motion != 0 && position != 0 && target != 0) {
        motion->position = position;
        motion->base = *target;
        motion->radius = xAmplitude;
        motion->radius2 = yAmplitude;
        motion->radius3 = zAmplitude;
        motion->speed = speed;
        motion->angle = angle;
        motion->mode = ORBIT_MOTION_WAVE_APPROACH;
        motion->state = 1;
        motion->timer = 0;
    }
}

s32 ScalarApproach_StepHalf(s32, s32);

s32 ScalarApproach_StepQuarter(s32, s32);

s32 ScalarApproach_StepEighth(s32, s32);

s32 ScalarApproach_StepSixteenth(s32, s32);

s32 Math_StepTowardTarget(s32, s32, s32);

s32 SceneMover_SteerXZ(struct OrbitMotion *);

s32 SceneMover_SteerXY(struct OrbitMotion *);

s32 OrbitMotion_UpdateAngle(struct OrbitMotion *);

s32 OrbitMotion_UpdateCrossedSines(struct OrbitMotion *);

void OrbitMotion_UpdateAbsoluteSines(struct OrbitMotion *);

s32 OrbitMotion_InterpolatePosition(struct OrbitMotion *);



s32 MotionTrack_Update(struct OrbitMotion *motion)
{
    struct OrbitVector *out;
    struct OrbitVector *target;
    struct OrbitVector *step;
    struct OrbitVector *savedTarget;
    struct OrbitVector *savedStep;
    s32 horizontalX, horizontalZ, stepDistanceSquared, stepYSquared, verticalY;
    u32 mode;
    if (motion->state != 1)
        return 0;
    out = motion->position;
    target = &motion->base;
    step = &motion->offset;
    mode = motion->mode;
    savedTarget = target;
    savedStep = step;
    switch (mode) {
        case ORBIT_MOTION_OFFSET:
            add(out, step, out);
            break;
        case ORBIT_MOTION_HALF:
            out->x = ScalarApproach_StepHalf(out->x, target->x);
            out->y = ScalarApproach_StepHalf(out->y, target->y);
            out->z = ScalarApproach_StepHalf(out->z, target->z);
            break;
        case ORBIT_MOTION_QUARTER:
            out->x = ScalarApproach_StepQuarter(out->x, target->x);
            out->y = ScalarApproach_StepQuarter(out->y, target->y);
            out->z = ScalarApproach_StepQuarter(out->z, target->z);
            break;
        case ORBIT_MOTION_EIGHTH:
            out->x = ScalarApproach_StepEighth(out->x, target->x);
            out->y = ScalarApproach_StepEighth(out->y, target->y);
            out->z = ScalarApproach_StepEighth(out->z, target->z);
            break;
        case ORBIT_MOTION_SIXTEENTH:
            out->x = ScalarApproach_StepSixteenth(out->x, target->x);
            out->y = ScalarApproach_StepSixteenth(out->y, target->y);
            out->z = ScalarApproach_StepSixteenth(out->z, target->z);
            break;
        case ORBIT_MOTION_STEP: {
            s16 value;
            out->x = Math_StepTowardTarget(out->x, target->x, step->x);
            value = Math_StepTowardTarget(out->y, target->y, step->y);
            out->y = value;
            out->z = Math_StepTowardTarget(out->z, target->z, step->z);
            break;
        }
        case ORBIT_MOTION_STEER_XZ: {
            s32 current;
            current = out->y;
            out->y = ScalarApproach_StepEighth(current, target->y);
            return SceneMover_SteerXZ(motion);
        }
        case ORBIT_MOTION_STEER_XY:
            return SceneMover_SteerXY(motion);
        case ORBIT_MOTION_ARC:
            return OrbitMotion_UpdateAngle(motion);
        case ORBIT_MOTION_CROSSED_SINES:
            return OrbitMotion_UpdateCrossedSines(motion);
        case ORBIT_MOTION_WAVE_APPROACH:
            OrbitMotion_UpdateAbsoluteSines(motion);
            {
                s16 value;
                out->x = Math_StepTowardTarget(out->x, target->x, step->x);
                value = Math_StepTowardTarget(out->y, target->y, step->y);
                out->y = value;
                out->z = Math_StepTowardTarget(out->z, target->z, step->z);
                break;
            }
        case ORBIT_MOTION_INTERPOLATE:
            return OrbitMotion_InterpolatePosition(motion);
    }
    if (target) {
        stepDistanceSquared = square(motion->offset.x) + square(savedStep->z);
        out = motion->position;
        horizontalX = motion->base.x - out->x;
        horizontalZ = savedTarget->z - out->z;
        if (square(horizontalX) + square(horizontalZ) <= stepDistanceSquared) {
            stepYSquared = square(savedStep->y);
            verticalY = out->y - motion->base.y;
            if (square(verticalY) <= stepYSquared) {
                motion->state = 2;
                motion->timer++;
                return 1;
            }
        }
    }
    motion->timer++;
    return 0;
}
