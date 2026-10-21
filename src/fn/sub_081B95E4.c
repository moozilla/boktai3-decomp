// MATCHING_CONCESSION: register-pinning
#include "orbit_motion.h"
#ifdef NONMATCHING
// fuzzy: 87.7%
// Remaining differences: saved-pointer placement and squared-distance register allocation.
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
static inline s32 Distance(struct OrbitVector *a, struct OrbitVector *b)
{
    s32 x = a->x - b->x;
    s32 z = a->z - b->z;
    return Square(x) + Square(z);
}
s32 MotionTrack_Update(struct OrbitMotion *argument)
{
    struct OrbitMotion *p = argument;
    struct OrbitVector *out;
    register struct OrbitVector *target asm("r5");
    register struct OrbitVector *step asm("r6");
    register struct OrbitVector *savedTarget asm("r9");
    register struct OrbitVector *savedStep asm("r8");
    s32 a, b, c, d;
    if (p->state != 1)
        return 0;
    out = p->position;
    target = &p->base;
    step = &p->offset;
    switch (({
        u32 k = p->unk28;
        savedTarget = target;
        savedStep = step;
        k;
    })) {
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
        case 6:
            out->y = ScalarApproach_StepEighth(out->y, target->y);
            return SceneMover_SteerXZ(p);
        case 7:
            return SceneMover_SteerXY(p);
        case 8:
            return OrbitMotion_UpdateAngle(p);
        case 9:
            return OrbitMotion_UpdateCrossedSines(p);
        case 10:
            OrbitMotion_UpdateAbsoluteSines(p);
        case 5:
            out->x = Math_StepTowardTarget(out->x, target->x, step->x);
            out->y = Math_StepTowardTarget(out->y, target->y, step->y);
            out->z = Math_StepTowardTarget(out->z, target->z, step->z);
            break;
        case 12:
            return OrbitMotion_InterpolatePosition(p);
    }
    if (target) {
        c = Square(p->offset.x) + Square(savedStep->z);
        out = p->position;
        a = p->base.x - out->x;
        b = savedTarget->z - out->z;
        if (Square(a) + Square(b) <= c) {
            d = Square(savedStep->y);
            a = out->y - p->base.y;
            if (Square(a) <= d) {
                p->state = 2;
                p->timer++;
                return 1;
            }
        }
    }
    p->timer++;
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", MotionTrack_Update);
#endif
