#define VariantSprite_SelectAnimation sub_08160550
#define Sprite_ApplyAnimation sub_08214588
#include "global.h"
struct VariantSprite {
    u8 task[0x18];
    u8 animation[0x1C];
    u8 sprite[0x44];
};
s32 AnimData_Load(void *, u32);
void sub_08214588(void *, void *);
void sub_08160550(struct VariantSprite *arg, u32 variant)
{
    register struct VariantSprite *p = arg;
    switch (variant) {
    case 0:
        AnimData_Load(p->animation, 0xB250);
        sub_08214588(p->sprite, p->animation);
        break;
    case 1:
        AnimData_Load(p->animation, 0xB290);
        sub_08214588(p->sprite, p->animation);
        break;
    case 2:
        AnimData_Load(p->animation, 0xB270);
        sub_08214588(p->sprite, p->animation);
        break;
    case 3:
        AnimData_Load(p->animation, 0xB2B0);
        sub_08214588(p->sprite, p->animation);
        break;
    case 4:
        AnimData_Load(p->animation, 0xB2D0);
        sub_08214588(p->sprite, p->animation);
        break;
    }
}
