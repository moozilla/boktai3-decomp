#include "global.h"
void sub_08122DA0(void);
extern u8 *gUnk_0200019C;
u32 sub_0806F558(u8 *p)
{
    sub_08122DA0();
    gUnk_0200019C = p;
    return 0;
}
