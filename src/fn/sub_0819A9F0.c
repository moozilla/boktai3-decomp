#include "global.h"

void SceneBurstEmitter_SetState(u8 *, u32);

void SceneBurstEmitter_TransitionIfComplete(u8 *p, u32 v)
{
    if (*(p + 0x3B) == 0xff) {
        SceneBurstEmitter_SetState(p, v);
    }
}
