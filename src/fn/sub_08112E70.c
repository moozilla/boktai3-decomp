#include "global.h"
void SceneDebris_ReleaseSprite(u8 *);
void SceneDebris_ReleaseAnimation(u8 *);
void SceneDebris_ReleaseVisual(u8 *p)
{
    if (p[0x1b] == 0)
        SceneDebris_ReleaseSprite(p);
    else
        SceneDebris_ReleaseAnimation(p);
}
