#include "global.h"
struct S {
    u8 f[0x18];
    u32 a;
};
extern struct S *gUnk_020004B8;
u8 SceneTilemap_IsActive(u32);
void SceneTilemap_SetScrollSource(u32 x)
{
    if (SceneTilemap_IsActive(x))
        gUnk_020004B8->a = x;
}
