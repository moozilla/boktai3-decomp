#include "global.h"

#define Vanargand_InitTransientEffect sub_081AF640
#define Vanargand_ProjectEffectOffset sub_081AF540

struct VanargandEffectVec {
    s16 x, y, z;
};
struct VanargandEffectSprite {
    u32 flags;
    u8 pad4[3];
    u8 mode;
    u8 pad8[8];
    u16 scale;
    u8 pad12[0x1c - 0x12];
    struct VanargandEffectVec position;
    u8 pad24[0x2c - 0x24];
};

struct VanargandTransientEffect {
    u8 task[0x18];
    struct VanargandEffectSprite sprite;
    u8 animation[0x1c];
    u16 variant;
    u16 timer;
};
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void sub_081AF540(struct VanargandEffectVec *, s16, s16, s32);

s32 sub_081AF640(struct VanargandTransientEffect *effect, struct VanargandEffectVec *origin,
                 s32 variant)
{
    struct VanargandEffectVec offset;
    struct VanargandEffectVec *position;
    struct VanargandEffectSprite *sprite;
    AnimData_Load(effect->animation, 0x89cc);
    sprite = &effect->sprite;
    Sprite_Init(sprite, effect->animation, 0);
    sprite->scale = (effect->timer >> 4) + 6;
    effect->sprite.mode = 1;
    effect->sprite.flags |= 0x200;
    sub_081AF540(&offset, 0, 8, 0x40);
    position = &effect->sprite.position;
    position->x = origin->x + offset.x;
    position->y = origin->y + offset.y;
    position->z = origin->z + offset.z;
    effect->variant = variant;
    effect->timer = 1;
    if (effect->variant == 0)
        AnimData_SelectPaletteBank(effect->animation, 0x7d);
    else
        AnimData_SelectPaletteBank(effect->animation, 0x7e);
}
