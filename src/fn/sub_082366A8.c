#include "global.h"
#include "render.h"

struct GlowVector {
    s16 x, y, z, pad;
};
struct GlowAnim {
    u8 pad0[6];
    u16 id;
    u8 pad8[4];
    void *colors;
};
struct GlowSprite {
    u32 flags;
    u8 pad4[3];
    u8 f07;
    u8 pad08[8];
    u16 f10;
    u8 pad12[0xA];
    struct GlowVector position;
    u8 pad24[0x24];
    u8 *palette;
    u16 colors[16];
    u8 f6C;
    u8 pad6D[0x7];
    void *owner;
};
struct GlowUnit {
    u8 pad0[0x18];
    u8 kind;
    u8 pad19[0xC0 - 0x19];
    struct GlowSprite sprite;
};
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);

void sub_082366A8(struct GlowUnit *p, struct GlowVector *pos)
{
    struct GlowSprite *sprite = &p->sprite;
    u32 variant;
    u32 id;
    struct GlowAnim *anim;
    u16 *c;
    u16 *start;
    u16 *lo;
    u32 fill;
    u8 *flag;
    if (p->kind == 0) {
        variant = 0;
        id = 0xA2;
    } else {
        variant = 1;
        id = 0xA1;
    }
    anim = (struct GlowAnim *)((u8 *)sprite + 0x2C);
    AnimData_Load(anim, 0xBA8);
    Sprite_Init(sprite, anim, 0);
    sprite->f10 = variant;
    sprite->f07 = 3;
    sprite->position = *pos;
    sprite->position.z += 0x18;
    sprite->flags |= 0x400;
    sprite->palette = gPaletteBankData + id * 32;
    fill = 0x3DEF;
    start = &sprite->colors[0];
    flag = &sprite->f6C;
    lo = start;
    c = &sprite->colors[15];
    do {
        *c = fill;
        c--;
    } while ((s32)c >= (s32)lo);
    anim->id = id;
    anim->colors = start;
    sprite->owner = p;
    *flag = 1;
}
