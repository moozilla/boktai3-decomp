#include "global.h"
#include "ram.h"
void ScreenOrbitEffect_DestroySlot(void *, void *);
u32 ScreenOrbitEffect_Destroy(u8 *s)
{
    u8 *p = s + 0x20;
    s32 i = 3;
    u32 z;
    do {
        ScreenOrbitEffect_DestroySlot(s, p);
        p += 0x38;
        i--;
    } while (i >= 0);
    z = 0;
    gEffectQuad = z;
    return 0;
}
