// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"
#include "engine.h"

u32 Player_CollectCallbackFlags(struct Player *arg)
{
    struct Player *p = arg;
    u32 flags = 0;
    register s32 i asm("r6") = 0;
    void **callback = p->flagCallbacks;
    do {
        void *cb = *callback;
        if (cb != NULL)
            flags |= Callback_InvokeActor(p, cb);
        callback++;
        i++;
    } while (i <= 2);
    return flags;
}
