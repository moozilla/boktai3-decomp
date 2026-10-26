#include "orbit_background.h"
#include "render.h"
extern u16 gUnk_02000528;
void *ScriptProc_Start(u16, u16);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, void *);
void Bg_SetPriority(u32, u32);
void OrbitBackground_LoadResource(struct OrbitBackground *p)
{
    u32 z[1];
    u32 f, v;
    p->resource = ScriptProc_Start(0xC091, 0xB12);
    z[0] = 0;
    Bg_SetupLayers(3, 2, p->resource, 0, 0, 1, z);
    Bg_SetPriority(1, 1);
    v = gUnk_02000528;
    f = 0;
    if (v == 1)
        f = 1;
    if (f != 0) {
        u16 *r = &gDispCntShadow;
        u32 m = ~0x200;
        *r = *r & m;
        p->specialMode = 1;
    } else {
        if (p->enabled != 0) {
            u32 m = 0x200;
            gDispCntShadow |= m;
        } else {
            u16 *r = &gDispCntShadow;
            u32 m = ~0x200;
            *r = *r & m;
        }
        p->specialMode = 0;
    }
}
