#include "global.h"

struct S {
    u8 f[0x50];
    s32 a;
    u32 b;
};
void SceneCommands_Dispatch(struct S *);
void Camera_RequestShake(u32);
u32 SceneCommands_TaskUpdate(struct S *p)
{
    SceneCommands_Dispatch(p);
    if (p->a != 0) {
        if (p->a > 0)
            p->a--;
        Camera_RequestShake(p->b);
    }
    return 0;
}
