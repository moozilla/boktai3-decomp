#include "global.h"
void Vanargand_SetPartAnimationParameter(u8 *p, u32 v)
{
    if (p) {
        *(u16 *)(p + 0x14A) = v;
        *(u16 *)(p + 0x366) = v;
    }
}
