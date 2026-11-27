#include "global.h"

void Otenko_UpdateMode(void *, u32);

s32 Otenko_OnModeUpdate(void *p, u32 mode)
{
    Otenko_UpdateMode(p, mode);
    return 0;
}
