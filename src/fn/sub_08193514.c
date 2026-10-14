#include "global.h"
#include "engine.h"

extern u16 gUnk_03004BD8;
void sub_082161B4(int, int, void *, int, int, int, void *);
void sub_08215A74(u32, u32);

static inline void Or16(u32 m, u16 *p)
{
    *p |= m;
}

void sub_08193514(u8 *t)
{
    u32 index;
    *(void **)(t + 0x50) = ScriptProc_Start(0xc091, 0xe356);
    index = 0;
    sub_082161B4(2, 0, *(void **)(t + 0x50), 0, 0, 1, &index);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    sub_08215A74(3, 2);
    index = 2;
    sub_082161B4(3, 0, *(void **)(t + 0x50), 0, 0, 1, &index);
    BgGfx_LoadTilemapSimple(1, 0, 0, 0, 0);
    Or16(0xc00, &gUnk_03004BD8);
    *(u16 *)(t + 0x5416) = 0;
    *(u16 *)(t + 0x5418) = 0;
    t[0x724b] = 0;
}
