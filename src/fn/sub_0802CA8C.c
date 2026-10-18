#include "global.h"

struct S {
    u8 f[0x18];
    u32 a[1];
};
struct S *ActorPool_GetManager(void);
u32 Actor_GetPartyTarget(u32 i)
{
    struct S *p = ActorPool_GetManager();
    if (!p)
        return 0;
    return p->a[i];
}
