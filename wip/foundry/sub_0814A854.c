#define Player_CollectCallbackFlags sub_0814A854
#include "global.h"
#include "player.h"
s32 sub_0824923C(void *, void *);
s32 sub_0814A854(struct Player *p)
{
    s32 i;
    s32 flags;
    void **callback;
    flags = 0;
    i = 0;
    callback = p->flagCallbacks;
loop:
    if (*callback)
        flags = sub_0824923C(p, *callback) | flags;
    callback++;
    i++;
    if (i <= 2)
        goto loop;
    return flags;
}
