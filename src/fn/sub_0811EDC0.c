#include "global.h"
void Modifier_UpdateIdleDrain(void);
struct S {
    void (*fn)(void);
    u32 a;
    u16 h;
    u16 b;
    u8 pad;
    u8 c;
    u8 d;
    u8 f;
};
void Modifier_InitDrainState(struct S *p, u32 a, u16 b, u8 c)
{
    u8 z = 0;
    p->f = z;
    p->h = z;
    p->b = b;
    p->c = c;
    p->a = a;
    p->fn = Modifier_UpdateIdleDrain;
    p->d = 1;
}
