#include "global.h"
extern u8 *gUnk_02000710;
void sub_0822CE00(s32 i, u16 v)
{
    *(u16 *)(gUnk_02000710 + i * 2 + 0x838) = v;
}
