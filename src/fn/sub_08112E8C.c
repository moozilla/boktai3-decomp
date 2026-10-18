#include "global.h"
void SceneDebris_ReleaseVisual(u8 *);
void SceneDebris_OnMotionEnd(u8 *p)
{
    if (p[0x1a]) {
        SceneDebris_ReleaseVisual(p);
        p[0x19] = 0;
        p[0x1a] = 0;
    }
}
