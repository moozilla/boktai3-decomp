#include "global.h"

struct S {
    u8 f[0x78];
    u8 a;
};
extern u32 gUnk_0200047C;
void SceneMessages_UnregisterReceiver(void *);
u32 SceneMessages_Destroy(struct S *p)
{
    SceneMessages_UnregisterReceiver(&p->a);
    return gUnk_0200047C = 0;
}
