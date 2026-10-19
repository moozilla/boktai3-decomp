#include "global.h"

struct Unk38Holder {
    u8 unknown0[0x38];
    u16 *colorSrc;
};

struct Unk8E8Buffers {
    u8 unknown0[0x888];
    u16 ampValues[16];
    u16 colorA[32];
    u16 colorB[32];
};

struct ActorX {
    u8 unknown0[4];
    struct Unk38Holder *holder;
    u8 unknown8[0x3D0 - 8];
    struct Unk8E8Buffers *buf;
};

void sub_080FB8E8(struct ActorX *actor)
{
    struct Unk8E8Buffers *buf = actor->buf;
    s8 k2 = 16;
    struct Unk38Holder *holder = actor->holder;
    u16 *colorSrc = holder->colorSrc;
    int i;

    for (i = 0; i <= 15; i++) {
        u16 new_var = 0x1F;
        buf->ampValues[i] = 0;
        buf->colorA[i] = new_var;
        buf->colorA[i + k2] = 0x7C00;
        buf->colorB[i] = 0x7FFF;
        buf->colorB[i + 16] = colorSrc[i];
    }
}
