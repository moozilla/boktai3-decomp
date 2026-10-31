// MATCHING_CONCESSION: register-pinning
#include "orbit_motion.h"
s32 ScalarApproach_StepHalf(s32, s32);
s32 ScalarApproach_StepQuarter(s32, s32);
s32 ScalarApproach_StepEighth(s32, s32);
s32 ScalarApproach_StepSixteenth(s32, s32);
s32 Math_StepTowardTarget(s32, s32, s32);
s32 SceneMover_SteerXZ(struct OrbitMotion *);
s32 SceneMover_SteerXY(struct OrbitMotion *);
s32 OrbitMotion_UpdateAngle(struct OrbitMotion *);
s32 OrbitMotion_UpdateCrossedSines(struct OrbitMotion *);
s32 OrbitMotion_UpdateAbsoluteSines(struct OrbitMotion *);
s32 OrbitMotion_InterpolatePosition(struct OrbitMotion *);
static inline s32 Square(s32 a)
{
    return a * a;
}
s32 MotionTrack_Update(struct OrbitMotion *argument)
{
    struct OrbitMotion *p = argument;
    struct OrbitVector *out;
    struct OrbitVector *target;
    struct OrbitVector *step;
    struct OrbitVector *savedTarget;
    struct OrbitVector *savedStep;
    s32 horizontalX, horizontalZ, stepDistanceSquared, stepYSquared, verticalY;
    u32 mode;
    if (p->state != 1)
        return 0;
    out = p->position;
    target = &p->base;
    step = &p->offset;
    mode = p->unk28;
    savedTarget = target;
    savedStep = step;
    switch (mode) {
        case 0:
            add(out, step, out);
            break;
        case 1:
            out->x = ScalarApproach_StepHalf(out->x, target->x);
            out->y = ScalarApproach_StepHalf(out->y, target->y);
            out->z = ScalarApproach_StepHalf(out->z, target->z);
            break;
        case 2:
            out->x = ScalarApproach_StepQuarter(out->x, target->x);
            out->y = ScalarApproach_StepQuarter(out->y, target->y);
            out->z = ScalarApproach_StepQuarter(out->z, target->z);
            break;
        case 3:
            out->x = ScalarApproach_StepEighth(out->x, target->x);
            out->y = ScalarApproach_StepEighth(out->y, target->y);
            out->z = ScalarApproach_StepEighth(out->z, target->z);
            break;
        case 4:
            out->x = ScalarApproach_StepSixteenth(out->x, target->x);
            out->y = ScalarApproach_StepSixteenth(out->y, target->y);
            out->z = ScalarApproach_StepSixteenth(out->z, target->z);
            break;
        case 6: {
            /* Reserve the scratch register across the signed coordinate load. */
            register s32 scratch asm("r3");
            s32 current;
            current = out->y;
            asm volatile("" : : "r"(scratch));
            out->y = ScalarApproach_StepEighth(current, target->y);
            return SceneMover_SteerXZ(p);
        }
        case 7:
            return SceneMover_SteerXY(p);
        case 8:
            return OrbitMotion_UpdateAngle(p);
        case 9:
            return OrbitMotion_UpdateCrossedSines(p);
        case 10:
            OrbitMotion_UpdateAbsoluteSines(p);
        case 5: {
            s16 value;
            out->x = Math_StepTowardTarget(out->x, target->x, step->x);
            value = Math_StepTowardTarget(out->y, target->y, step->y);
            out->y = value;
            out->z = Math_StepTowardTarget(out->z, target->z, step->z);
            break;
        }
        case 12:
            return OrbitMotion_InterpolatePosition(p);
    }
    if (target) {
        stepDistanceSquared = Square(p->offset.x) + Square(savedStep->z);
        out = p->position;
        horizontalX = p->base.x - out->x;
        horizontalZ = savedTarget->z - out->z;
        if (Square(horizontalX) + Square(horizontalZ) <= stepDistanceSquared) {
            stepYSquared = Square(savedStep->y);
            verticalY = out->y - p->base.y;
            if (Square(verticalY) <= stepYSquared) {
                p->state = 2;
                p->timer++;
                return 1;
            }
        }
    }
    p->timer++;
    return 0;
}
