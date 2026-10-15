#include "scalar_approach.h"
s32 ScalarApproach_StepQuarter(s32 current, s32 target)
{
    return ScalarApproach_Shift2(current, target);
}
