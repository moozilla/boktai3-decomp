#include "global.h"
struct G {
    u8 pad[0x30];
    s16 x;
    s16 padx;
    s16 y;
};
struct V {
    s16 x, y, z;
};
struct O {
    u8 pad[0x98];
    s16 x;
    s16 y;
    s16 z;
};
struct M {
    u8 pad[0x1a];
    u8 mode;
    u8 pad2[0x255 - 0x1b];
    u8 sub;
};
extern struct M *gUnk_02000488;
extern struct V *sub_08049B6C(void);
s32 sub_0805238C(struct O *o)
{
    struct V *v = sub_08049B6C();
    if (v != 0 && gUnk_02000488->mode == 4 && gUnk_02000488->sub <= 4) {
        s32 d = v->y - o->y;
        if (d < 0)
            d = -d;
        if (d <= 0xff) {
            s32 dx = v->x - o->x;
            s32 dz = v->z - o->z;
            if (dx <= 0x17c && dz <= 0x17c) {
                if (dx * dx + dz * dz <= 0x23410)
                    return 1;
            }
        }
    }
    return 0;
}
