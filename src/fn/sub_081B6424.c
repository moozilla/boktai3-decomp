#include "global.h"
void Vanargand_SetPairedEffectParameter(u8 *p, u32 v)
{
    *(u16 *)(p + 0x43E) = v;
    *(u16 *)(p + 0x222) = v;
}
