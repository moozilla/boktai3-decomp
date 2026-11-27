#include "global.h"
#include "scene/areas/area_effect.h"


s32 AreaEffect_InitializeLink(struct AreaEffectManager *p)
{
    p->count = 0;
    p->graphicsId = 0x1C7;
    p->visible = 0;
    gAreaEffectManager = p;
    return 0;
}
