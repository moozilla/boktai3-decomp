#include "global.h"
#include "engine.h"
#include "render.h"

struct AnimData081979B4 {
    u8 pad0[6];
    u16 id;
    u8 pad8[4];
    u8 *tiles;
};
struct Anim081979B4 {
    u8 pad0[0xc];
    struct AnimData081979B4 *data;
};
struct Sprite081979B4 {
    u8 pad0[0x3a];
    u16 id;
    u8 pad3c[0xc];
    u8 *tiles;
};
struct Task081979B4 {
    u8 pad0[0x18];
    u32 target;
    u8 b1c, b1d, b1e, b1f;
    u16 id;
    u8 pad22[2];
    u8 *tiles;
};

void PaletteEffect_ApplyBufferedPalette(struct Task081979B4 *t, s32 frame)
{
    u32 target = t->target;
    struct Anim081979B4 *anim = (target & 1) ? NULL : (struct Anim081979B4 *)target;
    struct Sprite081979B4 *sprite = (target & 1) ? (struct Sprite081979B4 *)(target & ~1) : NULL;
    u8 *tiles = (u8 *)t - (frame * 32 - 0x28);
    if (anim) {
        anim->data->id = 0x8000 | t->id;
        anim->data->tiles = tiles;
    } else if (sprite) {
        sprite->id = 0x8000 | t->id;
        sprite->tiles = tiles;
    }
}
