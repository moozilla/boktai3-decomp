#include "global.h"
#include "background_controller.h"
struct S {
    u8 f[0x28];
    s32 a;
};
s32 SceneTilemap_GetLayoutLength(void)
{
    if (!((struct S *)gSceneCamera))
        return -1;
    else
        return ((struct S *)gSceneCamera)->a;
}
