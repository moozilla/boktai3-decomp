#include "global.h"
#include "ram.h"

struct E {
    u8 f[4];
    u8 a;
    u8 g[0x6b];
    u8 b;
    u8 h[0x77];
};
struct A {
    u8 f[0x64];
    struct E e[16];
};
void Anim_Remove(void *);
void ScaledShadow_Destroy(void *);

void Hresvelgr_DestroyProjectilePool(struct A *p)
{
    s32 i;
    struct E *e = p->e;
    for (i = 15; i >= 0; i--) {
        if (e->a != 0)
            Anim_Remove(e);
        if (e->b != 0)
            ScaledShadow_Destroy(&e->b);
        e++;
    }
    gHresvelgrProjectilePool = 0;
}
