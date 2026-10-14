#include "global.h"
#include "engine.h"

struct Anim0819792C {
    u8 pad0[0xc];
    struct {
        u8 pad0[6];
        u16 id;
        u8 pad8[4];
        u8 *tiles;
    } *data;
};
struct Sprite0819792C {
    u8 pad0[0x3a];
    u16 id;
    u8 pad3c[0xc];
    u8 *tiles;
};
struct Task0819792C {
    u8 pad0[0x18];
    u32 target;
    u8 b1c, b1d, b1e, b1f;
    u16 id;
    u8 pad22[2];
    u8 *tiles;
    u8 buf[1];
};

struct Task0819792C *sub_0819792C(struct Anim0819792C *anim, struct Sprite0819792C *sprite,
                                  u32 hold, s32 count, s32 start)
{
    struct Task0819792C *t = Task_Create(10, count * 32 + 0x28);
    if (!t)
        return NULL;
    if (anim) {
        t->target = (u32)anim;
        t->id = anim->data->id;
        t->tiles = anim->data->tiles;
    } else if (sprite) {
        t->target = (u32)sprite | 1;
        t->id = sprite->id;
        t->tiles = sprite->tiles;
    } else {
        t->id = (u32)sprite;
        t->tiles = (u8 *)sprite;
    }
    t->b1c = 0;
    t->b1d = 0;
    t->b1e = hold;
    t->b1f = count;
    if (count > 0 && t->tiles)
        CpuFastSet(t->tiles + start * 32, t->buf, (count * 8) & 0x1fffff);
    return t;
}
