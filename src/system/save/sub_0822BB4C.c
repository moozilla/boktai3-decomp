#include "global.h"
u8 *Save_GetContinuationData(void);
u8 *Save_GetContinuationDataEnd(void);
s32 Save_GetContinuationSize(void)
{
    u8 *a = Save_GetContinuationData();
    return Save_GetContinuationDataEnd() - a;
}
