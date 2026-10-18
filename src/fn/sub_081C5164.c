#include "global.h"

struct S {
    u8 pad[0xd8];
    s32 fd8;
};
s32 Registry1909_UpdateVisibilityFlicker(u8 *, s32, s32);
void Registry1909_EnterHiddenState(struct S *);

void Registry1909_UpdateFlickerState(struct S *p)
{
    u8 *a = (u8 *)p + 0x18;
    s32 *q = &p->fd8;
    if (Registry1909_UpdateVisibilityFlicker(a, *q, 8)) {
        Registry1909_EnterHiddenState(p);
    } else {
        *q = *q + 1;
    }
}
