#include "player.h"
u32 Callback_InvokeActor(void *, void *);
u32 ActorFlags_GetResponseBits(struct Player *p)
{
    u32 result = 0;
    s32 i = 0;
    void **callback = p->flagCallbacks;
    for (;;) {
        if (*callback)
            result |= Callback_InvokeActor(p, *callback);
        callback++;
        if (++i > 2)
            return result;
    }
}
