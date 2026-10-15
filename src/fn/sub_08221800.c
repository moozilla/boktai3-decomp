#include "scalar_approach.h"
#define ScalarApproach_StepSixteenth sub_08221800
s32 ScalarApproach_StepSixteenth(s32 current, s32 target)
{
    return ScalarApproach_Shift4(current, target);
}
