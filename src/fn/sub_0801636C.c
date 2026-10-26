#include "global.h"
#include "ram.h"
void Effect_Remove(u8 *);
u32 FadingParticleManager_Destroy(u8 *p)
{
    s32 i = 0;
    do {
        u8 *e = p + 0x24 + i * 0x224;
        u32 t = e[0];
        s32 next = i + 1;
        if (t != 0) {
            s32 j = 0;
            u8 *q;
            if (j < e[1]) {
                q = e + 0x10;
                do {
                    Effect_Remove(q);
                    q += 0x44;
                    j++;
                } while (j < e[1]);
            }
        }
        i = next;
    } while (i <= 5);
    {
        u32 z = 0;
        gFadingParticleManager = z;
    }
    return 0;
}
