#include "global.h"

struct S {
    u8 pad[0xd4];
    void (*fd4)(void);
    u32 fd8;
};
void Registry1909_UpdateFlickerState(void);

void Registry1909_StartVisibilityFlicker(struct S *p)
{
    void (*f)(void) = Registry1909_UpdateFlickerState;
    p->fd4 = f;
    p->fd8 = 0;
}
