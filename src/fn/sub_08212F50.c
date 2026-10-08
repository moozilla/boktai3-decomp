#include "global.h"
struct G { u8 pad[0x596]; s16 f596; };
extern struct G *gUnk_02000710;
s32 sub_08212F50(s32 n)
{
    return gUnk_02000710->f596 & (1 << n);
}
