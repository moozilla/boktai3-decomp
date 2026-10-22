#include "global.h"
extern u8 *gUnk_030053F8;
u8 Save_ReadContinuation(u8);

u8 sub_0822BE60(void)
{
    return Save_ReadContinuation(gUnk_030053F8[8]);
}
