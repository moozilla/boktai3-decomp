#include "scene_indicator.h"

void sub_08011A18(struct SceneIndicators *owner, struct SceneIndicator *p)
{
    Sprite_Remove(&p->sprite);
}
