#include "global.h"
u8 *Save_GetContinuationData(void);
extern u8 *gUnk_02000710;
s32 Save_GetMainStateSize(void)
{
    return Save_GetContinuationData() - gUnk_02000710;
}
