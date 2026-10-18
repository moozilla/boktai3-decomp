#include "global.h"
struct S {
    u8 f[0x24];
    u32 a;
};
void ScenePaletteFade_SetState(void *, void (*)(void *));
void ScenePaletteFade_UpdateBlend(void *);
void ScenePaletteFade_BeginBlend(struct S *p)
{
    p->a = 0;
    ScenePaletteFade_SetState(p, ScenePaletteFade_UpdateBlend);
}
