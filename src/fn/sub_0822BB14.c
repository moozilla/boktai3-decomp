#include "global.h"
extern u8 *gUnk_02000710;
u8 *Save_GetContinuationDataEnd(void)
{
    return gUnk_02000710 + 0x858;
}
