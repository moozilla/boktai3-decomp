#include "global.h"
#include "render.h"
struct S {
    u8 f[0x24];
    u32 a24;
    u32 a28;
    u32 a2c;
};
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void PaletteScreen_LoadResources(void *);
void ScenePaletteFade_BeginDelay(void *);
u32 ScenePaletteFade_InitFromScript(struct S *p)
{
    s32 v;
    {
        u16 *g = &gDispCntShadow;
        u32 m = 0xFFFFF0FF;
        *g = *g & m;
    }
    PaletteScreen_LoadResources(p);
    if (Script_SeekToKeyword(0x66))
        v = Script_GetValue();
    else
        v = 6;
    p->a28 = v;
    v = Script_SeekToKeyword(0x73);
    if (v)
        v = Script_GetValue();
    p->a2c = v;
    p->a24 = 0;
    ScenePaletteFade_BeginDelay(p);
    return 0;
}
