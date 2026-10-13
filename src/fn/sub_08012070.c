#include "global.h"
struct Vec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void Sprite_Init(void *, void *, int);
void Collider_Init(void *, int, int, int, int, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void sub_0821FF58(void *, int, int, int, int, int);
void Anim_SetSequence(void *, void *, int, int, int);
void AnimatedHazard_Initialize(u8 *m, u8 *p)
{
    struct Vec size, offset;
    int zero;
    u8 *sprite = p + 0x64, *effect;
    Sprite_Init(sprite, m + 0x18, 1);
    zero = 0;
    sprite[7] = 2;
    Anim_SetSequence(p + 0x90, *(void **)(m + 0x34), 0, 0, zero);
    effect = p + 0x10;
    size.x = 64;
    size.y = 128;
    size.z = 64;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(effect, 0, 0x2001, 0, 16, &size, &offset);
    Collider_SetCallback(effect, 0, p);
    sub_0821FF58(effect, 100, 50, 0, 0x60000, 30);
    p[0] = zero;
}
