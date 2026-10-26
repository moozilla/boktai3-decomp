#include "global.h"
#include "ram.h"
struct E {
    u8 pad[0x38];
};
struct S {
    u8 f[0x18];
    struct E e[32];
    u8 g[0x734 - 0x18 - 0x700];
};
void Sprite_Remove(void *);
void SceneMessages_UnregisterReceiver(void *);
void SparkleEmitter_OnTaskDestroy(struct S *p)
{
    s32 i;
    struct E *e = p->e;
    u32 z;
    for (i = 31; i >= 0; i--) {
        Sprite_Remove(e);
        e++;
    }
    z = 0;
    SceneMessages_UnregisterReceiver((u8 *)p + 0x734);
    gSparkleEmitter = z;
}
