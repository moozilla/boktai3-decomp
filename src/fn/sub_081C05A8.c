#include "global.h"
#include "render.h"
struct S {
    u8 f[0x1c];
    u32 a;
    u32 b;
    u32 c;
    u32 d;
};
void ScenePaletteFade_BlendPaletteBanks(void *, u32, u32, u32, u32, u32);
void ScenePaletteFade_SetBackgroundOffsets(void *);
void ScenePaletteFade_BeginHold(void *);
void ScenePaletteFade_UpdateBlend(struct S *p)
{
    ScenePaletteFade_BlendPaletteBanks(gBgPaletteBuffer, p->a, p->b, p->c, p->d, 3);
    p->c = p->c + 1;
    ScenePaletteFade_SetBackgroundOffsets(p);
    if ((p->c >> p->d) != 0)
        ScenePaletteFade_BeginHold(p);
}
