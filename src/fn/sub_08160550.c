#include "global.h"
struct VariantSprite {
    u8 task[0x18];
    u8 animation[0x1C];
    u8 sprite[0x44];
};
s32 AnimData_Load(void *, u32);
void Sprite_ApplyAnimation(void *, void *);
void VariantSprite_SelectAnimation(struct VariantSprite *arg, u32 variant)
{
    register struct VariantSprite *p = arg;
    switch (variant) {
    case 0:
        AnimData_Load(p->animation, 0xB250);
        Sprite_ApplyAnimation(p->sprite, p->animation);
        break;
    case 1:
        AnimData_Load(p->animation, 0xB290);
        Sprite_ApplyAnimation(p->sprite, p->animation);
        break;
    case 2:
        AnimData_Load(p->animation, 0xB270);
        Sprite_ApplyAnimation(p->sprite, p->animation);
        break;
    case 3:
        AnimData_Load(p->animation, 0xB2B0);
        Sprite_ApplyAnimation(p->sprite, p->animation);
        break;
    case 4:
        AnimData_Load(p->animation, 0xB2D0);
        Sprite_ApplyAnimation(p->sprite, p->animation);
        break;
    }
}
