// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"
#include "engine.h"

u32 Player_CollectCallbackFlags(struct Player *arg)
{
    register struct Player *p asm("r5") = arg;
    u32 flags = 0;
    register s32 i asm("r6") = 0;
    register void **callback asm("r4") = p->flagCallbacks;
    do {
        void *cb = *callback;
        if (cb != NULL)
            flags |= Callback_InvokeActor(p, cb);
        callback++;
        asm("" ::"r"(i));
        i++;
    } while (i <= 2);
    return flags;
}
