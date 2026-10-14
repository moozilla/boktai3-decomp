#include "global.h"

extern u16 *gUnk_03001664;

u16 sub_08218C40(void)
{
    if (gUnk_03001664 == NULL)
        return 0;
    return *gUnk_03001664;
}
