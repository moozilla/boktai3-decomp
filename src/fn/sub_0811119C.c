#include "global.h"
struct P {
    u32 a, b, c, d;
};
struct Q {
    u8 f[0x18];
    struct P *p;
};
extern struct Q *gUnk_020004B8;
u8 SceneTilemap_IsActive(void);
void SceneTilemap_CopyScrollSourceState(struct P *dst)
{
    if (SceneTilemap_IsActive()) {
        *dst = *gUnk_020004B8->p;
    }
}
