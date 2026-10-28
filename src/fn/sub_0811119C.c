#include "global.h"
#include "background_controller.h"
struct P {
    u32 a, b, c, d;
};
struct Q {
    u8 f[0x18];
    struct P *p;
};
u8 SceneTilemap_IsActive(void);
void SceneTilemap_CopyScrollSourceState(struct P *dst)
{
    if (SceneTilemap_IsActive()) {
        *dst = *((struct Q *)gSceneCamera)->p;
    }
}
