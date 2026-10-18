#include "global.h"

void Nidhoggr_ChangeCompletedPartState(u32, u32, u32);

void Nidhoggr_SetPart23SpecialAnimation(u32 a)
{
    Nidhoggr_ChangeCompletedPartState(a, 0x17, 0x32);
}
