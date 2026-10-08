#include "global.h"
extern u8 *gUnk_02000610;
u8 *Script_GetPc(void);
u8 *sub_0821B478(u8 *, u32);
void sub_0821AAD8(u32 a)
{
    gUnk_02000610 = sub_0821B478(Script_GetPc(), a);
}
