#include "global.h"
#include "render.h"
// MATCHING_CONCESSION: register-pinning

struct AtVector {
    s16 x, y, z, pad;
};
struct AtAnim {
    u8 pad0[6];
    u16 id;
    u8 pad8[4];
    void *colors;
};
struct AtSprite {
    u32 flags;
    u8 pad4[3];
    u8 f07;
    u8 pad08[8];
    u16 f10;
    u8 pad12[0xA];
    struct AtVector position;
    u8 pad24[0x24];
    u8 *palette;
    u16 colors[16];
    u8 f6C;
};
struct AtUnit {
    u8 pad0[0x18];
    u32 pad18;
    struct AtVector offset;
    u8 kind;
    u8 pad25[7];
    struct AtSprite sprite;
};
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);

void AttachmentSprite_Setup(struct AtUnit *p0, struct AtVector *pos0)
{
    register struct AtUnit *p asm("r5") = p0;
    struct AtVector *pos = pos0;
    struct AtSprite *sprite = &p->sprite;
    u32 kind = p->kind;
    u32 id;
    register struct AtAnim *anim asm("r6");
    u16 *c;
    u16 *start;
    u16 *lo;
    u32 fill;
    u8 *flag;
    u32 v = kind;
    asm("" : "+r"(v));
    id = 0xA2;
    if (kind != 0) {
        id = 0xAB;
        if (kind == 1)
            id = 0xA1;
    }
    anim = (struct AtAnim *)((u8 *)sprite + 0x2C);
    AnimData_Load(anim, 0xBA8);
    Sprite_Init(sprite, anim, 0);
    sprite->f10 = v;
    sprite->f07 = 3;
    {
        struct AtVector *t = pos;
        sprite->position = *t;
    }
    sprite->position.x += p->offset.x;
    sprite->position.y += p->offset.y;
    sprite->position.z += p->offset.z;
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
    *flag = 1;
}
