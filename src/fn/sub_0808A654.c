#include "global.h"
extern const u32 gUnk_08605C9C[];
void Ghoul_SetActionHandlers(u8 *p)
{
    *(const u32 **)(p + 0x284) = gUnk_08605C9C;
}
