#include "global.h"
#include "engine.h"
#include "render.h"

struct AnimData08197A50 {
    u8 pad0[6];
    u16 id;
    u8 pad8[4];
    u8 *tiles;
};
struct Anim08197A50 {
    u8 pad0[0xc];
    struct AnimData08197A50 *data;
};
struct Sprite08197A50 {
    u8 pad0[0x3a];
    u16 id;
    u8 pad3c[0xc];
    u8 *tiles;
};
struct Task08197A50 {
    u8 pad0[0x18];
    u32 target;
    u8 b1c, b1d, b1e, b1f;
    u16 id;
    u8 pad22[2];
    u8 *tiles;
};

void PaletteEffect_UpdateSharedOverride(void *task)
{
    struct Task08197A50 *t = task;
    u32 target = t->target;
    struct Anim08197A50 *anim = (target & 1) ? NULL : (struct Anim08197A50 *)target;
    struct Sprite08197A50 *sprite = (target & 1) ? (struct Sprite08197A50 *)(target & ~1) : NULL;
    if (anim) {
        if (anim->data->id != 0x192) {
            t->id = anim->data->id;
            t->tiles = anim->data->tiles;
            anim->data->id = 0x192;
            {
                u8 *tiles = gPaletteBankData + 0x3240;
                anim->data->tiles = tiles;
            }
        }
    } else if (sprite) {
        if (sprite->id != 0x192) {
            t->id = sprite->id;
            t->tiles = sprite->tiles;
            sprite->id = 0x192;
            sprite->tiles = gPaletteBankData + 0x3240;
        }
    }
    if (t->b1e == 0)
        Task_Kill(task);
    else
        t->b1e--;
}
