#include "global.h"
#include "background_controller.h"
struct S {
    u8 pad[0x54];
    s32 x54;
};
u8 SceneTilemap_IsActive(void);

s32 sub_081111C4(void)
{
    if (!SceneTilemap_IsActive())
        return -1;
    else
        return ((struct S *)gSceneCamera)->x54 - 12;
}
