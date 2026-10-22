#include "scalar_approach.h"

s32 sub_08221AB8(s32 current, s32 target, s32 step)
{
    return ScalarApproach_Step(ScalarApproach_Unwrap(current, target), target, step);
}
