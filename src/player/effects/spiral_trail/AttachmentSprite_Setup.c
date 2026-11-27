#include "player/effects/spiral_trail_sprite.h"
#include "engine/video/render.h"
#include "scene/collision/contact_position.h"

struct AtUnit {
    u8 pad0[0x18];
    u32 pad18;
    struct ContactPosition offset;
    u8 kind;
    u8 pad25[7];
    struct SpiralTrailSprite sprite;
};
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);

void AttachmentSprite_Setup(struct AtUnit *owner, const struct ContactPosition *position)
{
    struct SpiralTrailSprite *sprite;
    struct SceneAnimation *anim;
    u16 *c, *start, *lo;
    u32 fill;
    u8 *flag;
    u32 id;
    u16 frame;
    sprite = &owner->sprite;
    frame = owner->kind;
    if ((u8)frame == 0) {
        id = 0xa2;
    } else if ((u8)frame == 1) {
        id = 0xa1;
    } else {
        id = 0xab;
    }
    anim = &sprite->graphics;
    AnimData_Load(anim, 0xBA8);
    Sprite_Init(&sprite->drawable, anim, 0);
    sprite->drawable.frame = frame;
    sprite->drawable.priority = 3;
    sprite->drawable.position = *position;
    sprite->drawable.position.x += owner->offset.x;
    sprite->drawable.position.y += owner->offset.y;
    sprite->drawable.position.z += owner->offset.z;
    sprite->drawable.flags |= 0x400;
    sprite->sourcePalette = (const u16 *)(gPaletteBankData + id * 32);
    fill = 0x3DEF;
    start = &sprite->colors[0];
    flag = &sprite->active;
    lo = start;
    c = &sprite->colors[15];
    do {
        *c = fill;
        c--;
    } while ((s32)c >= (s32)lo);
    anim->paletteIndex = id;
    anim->palette = start;
    *flag = 1;
}
