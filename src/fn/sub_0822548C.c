#include "global.h"
#include "runtime.h"
struct S {
    u8 pad[0xc];
    u32 fc;
    u32 f10;
    u16 f14;
    u16 f16;
};
void GameState_ResetSessionCounters(void)
{
    struct S *p = gSessionState;
    if (p) {
        p->fc = 0;
        p->f10 = 0;
        p->f14 = 0;
        p->f16 = 0;
    }
}
