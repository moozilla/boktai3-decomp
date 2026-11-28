#include "player/effects/spiral_trail.h"
#include "engine/video/render.h"
#include "scene/collision/contact_position.h"

void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);

void AttachmentSprite_Setup(struct SpiralTrailTask *owner, const struct ContactPosition *position)
{
    struct SpiralTrailSprite *sprite = &owner->sprite;
    struct SceneAnimation *anim;
    u32 id;
    u16 kind = owner->kind;
    u32 fill;
    s32 i;

    if ((u8)kind == 0)
        id = 0xa2;
    else if ((u8)kind == 1)
        id = 0xa1;
    else
        id = 0xab;
    anim = &sprite->graphics;
    AnimData_Load(anim, 0xBA8);
    Sprite_Init(&sprite->drawable, anim, 0);
    sprite->drawable.frame = kind;
    sprite->drawable.priority = 3;
    sprite->drawable.position = *position;
    sprite->drawable.position.x += owner->offset.x;
    sprite->drawable.position.y += owner->offset.y;
    sprite->drawable.position.z += owner->offset.z;
    sprite->drawable.flags |= 0x400;
    sprite->sourcePalette = (const u16 *)(gPaletteBankData + id * 32);
    fill = 0x3DEF;
    for (i = 0; i < 16; i++)
        sprite->colors[i] = fill;
    anim->paletteIndex = id;
    anim->palette = sprite->colors;
    sprite->active = 1;
}
