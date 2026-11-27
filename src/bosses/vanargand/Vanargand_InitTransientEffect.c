#include "global.h"
#include "scene/collision/contact_position.h"
#include "effects/bursts/fragment_effect_sprite_layout.h"

struct VanargandTransientEffect {
    u8 task[0x18];
    struct FragmentEffectSprite sprite;
    u8 animation[0x1c];
    u16 variant;
    u16 timer;
};
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Vanargand_ProjectEffectOffset(struct ContactPosition *, s16, s16, s32);

s32 Vanargand_InitTransientEffect(struct VanargandTransientEffect *effect,
                                  struct ContactPosition *origin, s32 variant)
{
    struct ContactPosition offset;
    struct ContactPosition *position;
    struct FragmentEffectSprite *sprite;
    AnimData_Load(effect->animation, 0x89cc);
    sprite = &effect->sprite;
    Sprite_Init(sprite, effect->animation, 0);
    sprite->frame = (effect->timer >> 4) + 6;
    effect->sprite.priority = 1;
    effect->sprite.flags |= 0x200;
    Vanargand_ProjectEffectOffset(&offset, 0, 8, 0x40);
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
#ifdef UBFIX
    // falls off the end with r0 left over from its last statement; the creator only tests
    // < 0, and this initializer has no failure path here
    return 0;
#endif
}
