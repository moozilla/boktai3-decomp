#include "global.h"
#include "ram.h"
void *AnimData_FindEntry(int);
void Effect_Initialize(void *, void *, int);
void Effect_SetOrigin(void *, int, int);
int ParticleManager_Initialize(u8 *m)
{
    int i;
    gParticleGroupManager = m;
    *(void **)(m + 28) = AnimData_FindEntry(0x1c1c);
    *(void **)(m + 32) = AnimData_FindEntry(0x1c1e);
    for (i = 0; i <= 5;) {
        int offset = i * 452;
        u8 *g = m + offset;
        u8 *s;
        int j, next;
        g += 36;
        g[0] = 0;
        next = i + 1;
        s = m + (offset + 56);
        for (j = 7; j >= 0; s += 56, j--) {
            Effect_Initialize(s, *(void **)(m + 28), 1);
            Effect_SetOrigin(s, -4, -4);
        }
        i = next;
    }
    *(u32 *)(m + 24) = 0;
    return 0;
}
