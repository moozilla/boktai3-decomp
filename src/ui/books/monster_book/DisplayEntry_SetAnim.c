#include "global.h"
#include "engine/video/sprite.h"

struct DispInfo {
    u8 pad[0x1C];
    s8 f1C;
    s8 f1D;
    u8 f1E;
    u8 f1F;
};

struct DispObj {
    u8 pad0[0xB4];
    u16 *fB4;
    u8 pad1[0x9A4 - 0xB8];
    struct DisplaySequenceSprite sprite;
    u8 pad4[0x9EE - 0x9D0];
    u16 f9EE;
    u8 pad5[0xA08 - 0x9F0];
    struct DispInfo *fA08;
};

void DisplayEntry_SetAnim(struct DispObj *p, s32 mode)
{
    struct DisplaySequenceSprite *sprite;

    switch (mode) {
        case 1:
            sprite = &p->sprite;
            sprite->frame = 5;
            break;
        case 2:
            sprite = &p->sprite;
            sprite->frame = 6;
            break;
        case 3:
            sprite = &p->sprite;
            sprite->frame = 7;
            break;
    }
    p->sprite.position.x = p->fB4[0] + p->fA08->f1C;
    p->sprite.position.y = p->f9EE + p->fA08->f1D;
    p->sprite.position.z = p->fB4[2];
    p->sprite.palette = p->fA08->f1E;
    p->sprite.paletteVariant = p->fA08->f1F;
}
