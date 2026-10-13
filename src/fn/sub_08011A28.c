#include "scene_indicator.h"
int SceneIndicators_Update(struct SceneIndicators *m)
{
    int i;
    struct SceneIndicator *p = m->indicators;
    for (i = 15; i >= 0; i--, p++)
        if (p->active)
            SceneIndicator_Update(m, p);
    {
        int x = gUnk_02000580->x >> 8, y = gUnk_02000580->y >> 8, tile;
        if (x < 0 || y < 0 || x >= gUnk_030052F8 || y >= gUnk_030052FC)
            tile = 0;
        else
            tile = gUnk_030052F4->rows[y] + x;
        m->tile = tile;
    }
    return 0;
}
