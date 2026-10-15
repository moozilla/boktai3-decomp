#include "scalar_approach.h"
#define ScalarApproach_StepQuarter sub_08221798
s32 ScalarApproach_StepQuarter(s32 current, s32 target)
{
    return ScalarApproach_Shift2(current, target);
}
