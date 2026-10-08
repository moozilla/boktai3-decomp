#include "global.h"
extern u32 gUnk_02000258;
void sub_081B80BC(void);
u32 sub_081B7E4C(void);
u32 sub_081B8160(void)
{
    if (gUnk_02000258 == 0)
        sub_081B80BC();
    return sub_081B7E4C();
}
