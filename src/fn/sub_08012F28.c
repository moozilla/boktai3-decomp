#include "global.h"
struct Collider {
    u8 pad[6];
    u16 flags;
};
static inline void disable(struct Collider *c)
{
    c->flags |= 4;
}
int sub_08012D20(void *);
void BreakableBarrier_Hide(void *m, u8 *p)
{
    if (sub_08012D20(p)) {
        *(u32 *)p |= 1;
        disable((void *)(p + 0x48));
    }
}
