#include "global.h"
struct S {
    u8 pad[0x892];
    s16 value;
};
extern struct S *volatile gUnk_02000710;

s16 sub_0811BB98(void)
{
    s32 v = gUnk_02000710->value;
    if (v > 999)
        v = 999;
    else if (v < 0)
        v = 0;
    gUnk_02000710->value = v;
    return gUnk_02000710->value;
}
