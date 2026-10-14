#include "global.h"
struct ContactActorPool;

struct S {
    u8 f[0x9d8];
    struct ContactActorPool *contactPool;
};
struct S *sub_0802140C(u32);
void ContactActorPool_RequestClear(struct ContactActorPool *);
void sub_08022590(void)
{
    struct S *p = sub_0802140C(0);
    if (p && p->contactPool != 0)
        ContactActorPool_RequestClear(p->contactPool);
}
