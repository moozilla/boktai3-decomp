#include "scene_indicator.h"

struct SceneIndicators *sub_08011C00(void)
{
    struct SceneIndicators *p;
    if (gUnk_0200004C != 0)
        return gUnk_0200004C;
    p = Task_Create(0x8, 0xbd0);
    if (p != 0) {
        Task_SetCallbacks(p, SceneIndicators_Update, sub_08011AA4);
        if (SceneIndicators_Initialize(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
