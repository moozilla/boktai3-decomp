#include "global.h"
extern u8 *gUnk_030053F8;
s32 sub_08243EB8(void);
/* Time left before the stored limit, clamped to 140; 0 if out of range. */
s32 sub_0822C388(void)
{
    s32 now = sub_08243EB8();
    s32 limit;
    if (now < 0 || now > (limit = *(s32 *)(gUnk_030053F8 + 4)))
        return 0;
    now = limit - now;
    if (now > 139)
        return 140;
    return now;
}
