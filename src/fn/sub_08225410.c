#include "global.h"
u8 *Script_GetPc(void);
void sub_0821B148(void);
u8 *sub_0821B5BC(u8 *);
s32 sub_08225410(void)
{
    u8 *p = Script_GetPc();
    if (p == 0) {
        sub_0821B148();
    } else {
        while (*p != 0)
            p = sub_0821B5BC(p);
    }
    return 0;
}
