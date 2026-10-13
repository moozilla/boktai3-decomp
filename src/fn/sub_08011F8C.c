#include "scene_indicator.h"
int SceneIndicator_Pause(void)
{
    struct SceneIndicators *m = sub_08011884();
    int id;
    struct SceneIndicator *p;
    if (!m)
        return -1;
    id = Script_GetKeywordValue('n', 0);
    if (!id)
        return -1;
    p = sub_08011C6C(m, id);
    if (!p)
        return -1;
    p->disabled = 1;
    p->visible = 0;
    IndicatorSprite_SetFrame(&p->sprite, p->frame);
    return 0;
}
