#include "global.h"
extern u8 *gUnk_02000710;
u32 Collection_IsEntryUnlocked(u32 i)
{
    return *(u32 *)(gUnk_02000710 + 0x728) & (1 << i);
}
