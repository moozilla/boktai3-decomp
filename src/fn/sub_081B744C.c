#include "global.h"
#include "render.h"

struct E {
    s32 v;
    u8 f[0x48];
};
struct A {
    u8 f[0x1c];
    struct E e[8];
};
void VanargandAfterimagePool_UpdateSlot(void *, void *, s32);

void VanargandAfterimagePool_Update(struct A *p)
{
    s32 i;
    struct E *e = p->e;
    if ((gEnginePauseFlags & 7) == 0) {
        for (i = 0; i <= 7; i++) {
            if (e->v > 0)
                VanargandAfterimagePool_UpdateSlot(p, &p->e[i], i);
            e++;
        }
    }
}
