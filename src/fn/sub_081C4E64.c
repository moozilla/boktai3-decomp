#include "global.h"

struct S {
    u8 pad[0x18];
    u32 f18;
};
u32 RegistryF3E4_GetStateResult(u32);
void sub_081C4DC4(struct S *);

void RegistryC281_UpdateSecondMovement(struct S *p)
{
    if (RegistryF3E4_GetStateResult(p->f18))
        sub_081C4DC4(p);
}
