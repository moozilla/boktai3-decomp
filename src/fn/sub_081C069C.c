#include "global.h"
struct S {
    u8 f[0x20];
    s16 a20;
    s16 a22;
    u8 g[4];
    s16 a28;
    s16 a2a;
};
void BgGfx_LoadTilemapAtPosition(u32, s32, s32);
void SceneActorSequence_UpdateBackgroundScroll(struct S *p)
{
    BgGfx_LoadTilemapAtPosition(1, p->a28, p->a2a);
    BgGfx_LoadTilemapAtPosition(3, p->a20, p->a22);
}
