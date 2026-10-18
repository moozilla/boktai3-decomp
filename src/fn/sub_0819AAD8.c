#include "global.h"

void SceneBurstEmitter_SetState(u8 *, u32);

void SceneBurstEmitter_RequestCleanup(u8 *p)
{
    if (p != 0 && *(p + 0x3A) == 0) {
        SceneBurstEmitter_SetState(p, 3);
    }
}
