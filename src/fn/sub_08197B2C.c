#include "global.h"
#include "render.h"
struct P {
    u8 f[0x3a];
    u16 h;
    u8 g[0xc];
    u32 w;
};
struct P *PaletteEffect_Create(s32, void *, s32, s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void PaletteEffect_UpdateSharedOverride(void);
void PaletteEffect_Destroy(void);
void PaletteEffect_StartSpriteOverride(struct P *p, s32 b, s32 c)
{
    struct P *r = PaletteEffect_Create(0, p, b, 0, c);
    if (r != 0) {
        u8 *q;
        Task_SetCallbacks(r, PaletteEffect_UpdateSharedOverride, PaletteEffect_Destroy);
        {
            u16 k = 0x192;
            q = gPaletteBankData;
            q += 0x3240;
            q -= c << 5;
            p->h = k;
            p->w = (u32)q;
        }
    }
}
