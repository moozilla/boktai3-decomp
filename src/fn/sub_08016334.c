#include "global.h"
#include "ram.h"
void FadingParticleGroup_Update(u8 *);
u32 FadingParticleManager_Update(u8 *s)
{
    u8 **gp = &gFadingParticleManager;
    u32 off = 0x24;
    s32 i = 5;
    u8 *e;
    do {
        e = *gp + off;
        if (*e != 0)
            FadingParticleGroup_Update(e);
        off += 0x224;
        i--;
    } while (i >= 0);
    *(u32 *)(s + 0x18) += 1;
    return 0;
}
