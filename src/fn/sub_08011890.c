#include "scene_indicator.h"
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void SceneIndicator_Initialize(struct SceneIndicators *m, struct SceneIndicator *p)
{
    struct PackedVec size, offset;
    int zero;
    u8 *sprite = (u8 *)&p->sprite;
    u8 *effect;
    Sprite_Init(sprite, m->animations, 1);
    zero = 0;
    sprite[7] = 3;
    effect = p->effect;
    size.x = 120;
    size.y = 32;
    size.z = 120;
    offset.x = 0;
    offset.y = 32;
    offset.z = 0;
    Collider_Init(effect, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetCallback(effect, 0, p);
    sub_0821FF58(effect, 10, 1, 0x400000, 0x60000, 30);
    p->active = zero;
}
