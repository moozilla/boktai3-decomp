#include "global.h"

s32 Div(s32, s32);
s32 Mod(s32, s32);
s32 RepeatedEffect_RequestNearby(void *);

struct Inner {
    u8 pad[4];
    s16 w;
};
struct Outer {
    u8 pad[4];
    struct Inner *in;
};
extern struct Outer *gTerrainMap;

struct Obj {
    u8 pad0[0x288];
    u16 idx;
    u8 pad1[0x47A - 0x28A];
    u8 flag;
    u8 busy;
    u16 *ptr;
    u16 x;
    u16 pad2;
    u16 y;
};

void Player_RequestNearbyEffectTarget(struct Obj *o)
{
    if (RepeatedEffect_RequestNearby((u8 *)o + 0x30) != 0 && o->busy == 0) {
        s32 m = Mod(o->idx, gTerrainMap->in->w);
        s32 d = Div(o->idx, gTerrainMap->in->w);
        o->x = (m << 8) + 0x80;
        o->y = (d << 8) + 0x80;
        o->ptr = &o->x;
        o->flag = 1;
    }
}
