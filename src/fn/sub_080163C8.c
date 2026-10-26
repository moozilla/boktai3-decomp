#include "global.h"
#include "ram.h"
void *AnimData_FindEntry(int);
void *ScriptProc_Start(int, int);
void Effect_Initialize(void *, void *, int);
void Anim_SetSequence(void *, void *, int, int, int);
int FadingParticleManager_Initialize(u8 *m)
{
    int i;
    int clear;
    gFadingParticleManager = m;
    *(void **)(m + 28) = AnimData_FindEntry(0x1c1a);
    *(void **)(m + 32) = ScriptProc_Start(0x922e, 0xd1b8);
    i = 0;
    clear = 0;
    do {
        u8 *g = m + 36 + i * 548;
        u8 *a, *s;
        int j, next;
        g[0] = clear;
        next = i + 1;
        a = g + 56;
        s = g + 16;
        for (j = 7; j >= 0; a += 68, s += 68, j--) {
            Effect_Initialize(s, *(void **)(m + 28), 0);
            Anim_SetSequence(a, *(void **)(m + 32), 3, 0, clear);
        }
        i = next;
    } while (i <= 5);
    *(u32 *)(m + 24) = 0;
    return 0;
}
