// MATCHING_CONCESSION: register-pinning
#include "scene_indicator.h"
int SceneIndicators_Initialize(struct SceneIndicators *m)
{
    int i;
    struct IndicatorAnimation *a;
    struct SceneIndicator *first;
    gUnk_0200004C = m;
    m->count = 0;
    if (Script_SeekToKeyword('t')) {
        for (i = 0, a = m->animations; i < 4; a++, i++) {
            int value;
            if (Script_GetPc()) {
                value = Script_GetValue();
                m->count++;
            } else
                value = 0x4211;
            if (!AnimData_Load(a, value))
                return -1;
        }
    } else {
        for (i = 0, a = m->animations; i < 4; a++, i++)
            if (!AnimData_Load(a, 0x4211))
                return -1;
        m->count = 4;
    }
    asm("" : "+r"(m));
    {
        int has = Script_SeekToKeyword('p');
        first = m->indicators;
        if (has) {
            for (a = m->animations, i = 3; i >= 0; a++, i--) {
                int value;
                if (Script_GetPc()) {
                    value = Script_GetValue();
                    m->count++;
                } else
                    value = 0;
                AnimData_SelectTiles(a, a->frame + value);
            }
        }
    }
    {
        struct SceneIndicator *p = first;
        for (i = 15; i >= 0; i--, p++)
            SceneIndicator_Initialize(m, p);
    }
    if (gUnk_02000580) {
        int x = gUnk_02000580->x >> 8, y = gUnk_02000580->y >> 8, tile;
        if (x < 0 || y < 0 || x >= gUnk_030052F8 || y >= gUnk_030052FC)
            tile = 0;
        else
            tile = gUnk_030052F4->rows[y] + x;
        m->tile = tile;
    }
    return 0;
}
