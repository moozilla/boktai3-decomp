#include "global.h"
struct E {
    u8 f[0x8a];
    u16 h;
    u8 g[0x90 - 0x8c];
};
struct P {
    u8 f[0x38];
    u32 n;
    u8 g[12];
    struct E e[32];
};
void Hresvelgr_TickShot(void *, void *, u32);
s32 Hresvelgr_UpdateShots(struct P *p)
{
    struct E *e = p->e;
    s32 i = 0;
    do {
        if (e->h != 0)
            Hresvelgr_TickShot(p, e, (p->n + i) & 3);
        e++;
        i++;
    } while (i <= 0x1f);
    p->n++;
}
