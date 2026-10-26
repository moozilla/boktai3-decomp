#include "global.h"
#include "render.h"

struct A {
    u8 f[0x28];
    u16 v;
    u8 g[0x38];
    u16 c;
};
void Task_Kill(void *);

void Vanargand_UpdateTransientEffect(struct A *p)
{
    u32 t = gEnginePauseFlags & 7;
    if (t == 0) {
        p->v = (p->c >> 4) + 6;
        p->c++;
        if (p->c == 0x50) {
            p->c = t;
            Task_Kill(p);
        }
    }
}
