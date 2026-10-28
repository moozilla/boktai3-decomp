#include "global.h"
#include "background_controller.h"
struct S {
    u8 f[0x18];
    u32 a;
};
u8 SceneTilemap_IsActive(u32);
void SceneTilemap_SetScrollSource(u32 x)
{
    if (SceneTilemap_IsActive(x))
        ((struct S *)gSceneCamera)->a = x;
}
