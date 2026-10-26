#include "global.h"
#include "engine.h"
#include "render.h"

struct AnimData08197A0C {
    u8 pad0[6];
    u16 id;
    u8 pad8[4];
    u8 *tiles;
};
struct Anim08197A0C {
    u8 pad0[0xc];
    struct AnimData08197A0C *data;
};
struct Sprite08197A0C {
    u8 pad0[0x3a];
    u16 id;
    u8 pad3c[0xc];
    u8 *tiles;
};
struct Task08197A0C {
    u8 pad0[0x18];
    u32 target;
    u8 b1c, b1d, b1e, b1f;
    u16 id;
    u8 pad22[2];
    u8 *tiles;
};

void PaletteEffect_Destroy(struct Task08197A0C *t)
{
    u32 target = t->target;
    struct Anim08197A0C *anim = (target & 1) ? NULL : (struct Anim08197A0C *)target;
    struct Sprite08197A0C *sprite = (target & 1) ? (struct Sprite08197A0C *)(target & ~1) : NULL;
    if (anim) {
        anim->data->id = t->id;
        anim->data->tiles = t->tiles;
    } else if (sprite) {
        sprite->id = t->id;
        sprite->tiles = t->tiles;
    }
}
