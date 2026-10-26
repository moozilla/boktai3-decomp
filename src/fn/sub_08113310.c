#include "global.h"
#include "ram.h"
struct E {
    u8 f[0x19];
    u8 b;
    u8 pad[6];
};
void SceneDebris_ReleaseVisual(void *);
s32 SceneDebrisPool_Destroy(u8 *s)
{
    s32 i;
    u32 z;
    struct E *p = (struct E *)(s + 0x18);
    i = 0x3f;
    do {
        if (p->b != 0)
            SceneDebris_ReleaseVisual(p);
        p++;
    } while (--i >= 0);
    z = 0;
    gDebrisPool = z;
    return 0;
}
