#include "global.h"
extern u32 gUnk_02000058;
void sub_08214514(u8 *);
void sub_080136DC(u32, u8 *);
u32 sub_08013B74(u8 *p)
{
    sub_08214514(p + 0xc);
    if (gUnk_02000058 != 0)
        sub_080136DC(gUnk_02000058, p);
    return 0;
}
