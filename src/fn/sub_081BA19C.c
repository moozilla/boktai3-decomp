#include "global.h"

struct Q {
    u8 f[0x520];
    s16 h520;
    u8 g[0x0];
    u8 pad[0];
};
struct Q2 {
    u8 f[0x522];
    u8 sh;
};
void Registry2388_BlendTimedPalettes(void *);
void Registry2388_SetUpdate(void *, void *);

void Registry2388_UpdateTimedEffect(struct Q *p)
{
    Registry2388_BlendTimedPalettes(p);
    p->h520++;
    if (p->h520 > (1 << ((struct Q2 *)p)->sh))
        Registry2388_SetUpdate(p, 0);
}
