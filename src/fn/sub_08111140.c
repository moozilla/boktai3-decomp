#include "global.h"
struct S {
    u8 f[0x28];
    s32 a;
};
extern struct S *gUnk_020004B8;
s32 SceneTilemap_GetLayoutLength(void)
{
    if (!gUnk_020004B8)
        return -1;
    else
        return gUnk_020004B8->a;
}
