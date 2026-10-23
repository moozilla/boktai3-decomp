#include "global.h"

// MATCHING_CONCESSION: register-pinning
#define ActorFlags_GetResponseBits sub_0814A854
u32 Callback_InvokeActor(void *, void *);
u32 ActorFlags_GetResponseBits(u8 *p)
{
    u32 result = 0;
    register s32 i asm("r6") = 0;
    void **callback = (void **)(p + 0xC8C);
    do {
        if (*callback)
            result |= Callback_InvokeActor(p, *callback);
        ++callback;
    } while (++i <= 2);
    return result;
}
