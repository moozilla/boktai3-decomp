#include "global.h"
extern u8 *gUnk_02000710;
static inline u8 TestFlag(u32 mask)
{
    if (*(u32 *)(gUnk_02000710 + 0x868) & mask)
        return 1;
    return 0;
}
u32 sub_0823B8C4(u32 flags)
{
    if ((flags & 0x80) && TestFlag(0x4000))
        return 1;
    return 0;
}
