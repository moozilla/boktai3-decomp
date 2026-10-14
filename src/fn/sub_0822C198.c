#include "global.h"
extern u8 *gUnk_03002604;
extern u8 *gUnk_030053F8;
s32 sub_08243EB8(void);
s32 sub_0822C198(void)
{
    if (gUnk_03002604[0x19] == 2) {
        s32 v = sub_08243EB8();
        if (v >= 0) {
            s32 w = v - 2;
            *(s32 *)(gUnk_030053F8 + 4) = w;
            return 1;
        }
    }
    return 0;
}
