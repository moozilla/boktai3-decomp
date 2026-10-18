#include "global.h"
s32 Modifier_EvaluateThirdSourceModeTwo(u8 *p)
{
    if (!p[0x10])
        return -1;
    else
        return 1;
}
