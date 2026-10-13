#include "global.h"
struct Motion {
    u32 id, flags;
    u8 pad[0x40];
};
struct Object {
    u8 pad[0x1b];
    u8 kind;
    u8 pad1c[0x40];
    struct Motion motion;
    u8 vector[8];
};
void sub_08225AB4(void *, int, int, int, int, void *);
int sub_08225B20(void *, void *);
int sub_0802D728(struct Object *p, int a, int b, int kind)
{
    struct Motion *m;
    int result;
    p->kind = kind;
    m = &p->motion;
    sub_08225AB4(m, a, b, (u8)kind, 7, p);
    result = sub_08225B20(m, p->vector);
    {
        u32 mask = 4;
        m->flags |= mask;
    }
    return result;
}
