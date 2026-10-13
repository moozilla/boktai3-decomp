#include "global.h"
// Tentative: Spread target searches across the manager's four update slots.
struct Vec {
    s16 x, y, z, pad;
};
struct Object {
    u8 pad0[9];
    u8 angle;
    u8 pada[2];
    struct Vec pos;
    u8 pad14[0x124];
    u32 range;
    u8 arc, pad13d[3];
    u32 distance;
    u8 delta;
    s8 target;
    u8 count, slot;
};
void *sub_0802C678(void);
struct Object *sub_0802CA8C(int);
int Math_Atan2(int, int);
struct Manager {
    u8 pad[0x30];
    u32 slot;
};
int sub_0802CAAC(struct Object *);
void sub_0802CB94(struct Object *);
void sub_0802CC4C(struct Object *p)
{
    struct Manager *m = sub_0802C678();
    int selected = 0;
    if (m->slot == p->slot)
        selected = 1;
    if (selected) {
        if (p->target >= 0)
            sub_0802CB94(p);
        else
            p->target = sub_0802CAAC(p);
    }
}
