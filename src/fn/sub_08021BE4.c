#include "global.h"

struct S {
    u8 f[0x54];
    u8 a;
};
struct S *SceneActors_FindAvailable(void);
u8 *SceneActors_GetAvailablePosition(void)
{
    struct S *p = SceneActors_FindAvailable();
    if (!p)
        return 0;
    return &p->a;
}
