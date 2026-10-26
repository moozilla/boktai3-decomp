#include "global.h"
#include "ram.h"
void AnimData_Load(void *, u32);
u32 BurstSpark_InitPool(u8 *p)
{
    AnimData_Load(p + 0x18, 0x8639);
    gBurstSparkPool = p;
    return 0;
}
