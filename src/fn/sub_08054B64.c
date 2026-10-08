#include "global.h"
extern u32 gUnk_02000490;
void sub_08054AD0(void);
u32 sub_08054834(void);
u32 sub_08054B64(void)
{
    if (gUnk_02000490 == 0)
        sub_08054AD0();
    return sub_08054834();
}
