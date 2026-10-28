#include "global.h"
#include "background_controller.h"
struct S {
    u8 f[0x44];
    s32 a;
    u8 g[4];
    s32 b;
};
s32 SceneTilemap_GetScrollDelta(void)
{
    struct S *s = gSceneCamera;
    if (!s)
        return 0;
    else
        return s->a - s->b;
}
