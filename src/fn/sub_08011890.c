#include "global.h"
struct Vec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void Sprite_Init(void *, void *, int);
void Collider_Init(void *, int, int, int, int, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void sub_0821FF58(void *, int, int, int, int, int);
void SceneIndicator_Initialize(u8 *m, u8 *p)
{
    struct Vec size, offset;
    int zero;
    u8 *sprite = p + 0x88, *effect;
    Sprite_Init(sprite, m + 0x1c, 1);
    zero = 0;
    sprite[7] = 3;
    effect = p + 0x34;
    size.x = 120;
    size.y = 32;
    size.z = 120;
    offset.x = 0;
    offset.y = 32;
    offset.z = 0;
    Collider_Init(effect, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetCallback(effect, 0, p);
    sub_0821FF58(effect, 10, 1, 0x400000, 0x60000, 30);
    p[16] = zero;
}
