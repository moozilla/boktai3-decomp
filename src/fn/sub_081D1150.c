#include "global.h"
#include "ram.h"
struct Z {
    u8 f[0x20];
};
struct Y {
    u8 f[0xc];
    u8 c;
};
void SceneNpc_RequestRigAction(void)
{
    u8 *p = gSceneNpc;
    struct Y *q;
    if (p) {
        q = (struct Y *)(p + 0x20);
        q->c = 1;
    }
}
