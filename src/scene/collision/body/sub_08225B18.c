#include "global.h"
struct S {
    u8 pad[0x1c];
    u32 f1c;
};
void SceneBody_ClearMovementProbe(struct S *p)
{
    p->f1c = 0;
}
