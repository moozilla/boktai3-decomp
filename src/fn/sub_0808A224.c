#include "global.h"
extern const u32 gUnk_08605C94[];
void Ghoul_SetBehaviorHandlers(u8 *p)
{
    *(const u32 **)(p + 0x288) = gUnk_08605C94;
}
