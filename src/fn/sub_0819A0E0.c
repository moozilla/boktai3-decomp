#include "global.h"

void Callback_InvokeActor(u8 *, u32);

void CannonBattery_UpdateMuzzleFlash(u8 *p)
{
    if (*(p + 0xB7)) {
        u32 z = 0;
        u32 q;
        *(u32 *)p = z;
        q = *(u32 *)(p + 4);
        if (q) {
            Callback_InvokeActor(p, q);
            *(u32 *)(p + 4) = z;
        }
    }
}
