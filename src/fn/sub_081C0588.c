#include "global.h"
struct S {
    u8 f[0x2c];
    u32 lim;
    u32 a;
    u32 cnt;
};
void ScenePaletteFade_SetBackgroundOffsets(void *);
void ScenePaletteFade_BeginBlend(void *);
void ScenePaletteFade_UpdateDelay(struct S *p)
{
    ScenePaletteFade_SetBackgroundOffsets(p);
    p->cnt = p->cnt + 1;
    if (p->cnt >= p->lim)
        ScenePaletteFade_BeginBlend(p);
}
