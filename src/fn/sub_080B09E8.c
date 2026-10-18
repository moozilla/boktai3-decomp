#include "global.h"
extern const u32 gUnk_08605F9C[];
void Vorn_InitAttackActionTable(u8 *p)
{
    *(const u32 **)(p + 0x284) = gUnk_08605F9C;
}
