#include "global.h"
#include "player.h"
#include "engine.h"

#ifdef NONMATCHING
// fuzzy: 95.7%
// foundry draft (worker best 95.7%); what differs is mostly register allocation and instruction order
#define Player_CollectCallbackFlags sub_0823D7E0

u32 sub_0823D7E0(struct Player *p)
{
    u32 flags = 0;
    u32 i = 0;
    void **callback;
    do {
        callback = &p->flagCallbacks[i];
        if (*callback != NULL)
            flags |= Callback_InvokeActor(p, *callback);
        i++;
    } while (i <= 2);
    return flags;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823D7E0);
#endif
