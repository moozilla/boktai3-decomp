#include "global.h"
extern u8 *gUnk_02000710;
u8 *Save_GetContinuationData(void)
{
    return gUnk_02000710 + 0x778;
}
