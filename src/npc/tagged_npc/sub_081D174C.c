#include "global.h"
void Trigger_ConsumePending(u8 *, u32);
void TaggedSceneNpc_UpdateMode18(u8 *a, u8 *p)
{
    if (p[8] != 0) {
        p[8] = 0;
        p[7] = 0;
        p[5] = 0xf;
    }
    if (p[7] != 0) {
        p[5] = 0x12;
        Trigger_ConsumePending(p + 0x168, 1);
    }
}
