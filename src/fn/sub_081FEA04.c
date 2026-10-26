#include "global.h"
#include "ram.h"
void SunOrb_GetOrCreateManager(void);

s32 sub_081FEA04(void)
{
    s32 result;
    u8 *m;
    if ((m = gSlotController) != NULL) {
        result = *(s32 *)(m + 0xef0);
    } else {
        SunOrb_GetOrCreateManager();
        result = -1;
    }
    return result;
}
