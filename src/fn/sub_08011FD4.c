#include "scene_indicator.h"
int SceneIndicator_Resume(void)
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
    if (!p || !p->disabled)
        return -1;
    p->disabled = 0;
    switch (p->mode) {
        case 0:
            p->index = 0;
            break;
        case 1:
            p->delay = 0;
            break;
        case 2:
            p->visible = 1;
            IndicatorSprite_SetFrame(&p->sprite, p->frame + 1);
            break;
    }
    return 0;
}
