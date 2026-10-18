#include "global.h"

void Callback_InvokeActor(u8 *p, u32 f);

u32 ScaledShadowManager_UpdateAll(u8 *p)
{
    u8 *n = *(u8 **)(p + 0x1C);
    if (n) {
        do {
            Callback_InvokeActor(n, *(u32 *)(n + 0x60));
            n = *(u8 **)(n + 0x68);
        } while (n);
    }
    return 0;
}
