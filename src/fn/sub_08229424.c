#include "global.h"
/* Clock_IsEarlyMorning: Tests whether the cached base hour is between midnight and two inclusive */
#define Clock_IsEarlyMorning sub_08229424
extern u8 gUnk_03005430[];
u32 sub_08229424(void)
{
    if (gUnk_03005430[4] <= 2)
        return 1;
    return 0;
}
