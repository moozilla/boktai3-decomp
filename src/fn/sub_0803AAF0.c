#include "global.h"
struct Drawing {
    u8 pad0[0x60];
};
struct Display {
    u8 pad0[0x40];
    struct Drawing drawings[4];
    u8 pad1c0[0x20];
    struct Drawing extra[2];
};
void Anim_Remove(void *);
int sub_0803AAF0(struct Display *p)
{
    struct Drawing *q = p->drawings;
    int i = 3;
    do {
        Anim_Remove(q);
        i--;
        q++;
    } while (i >= 0);
    q = p->extra;
    i = 1;
    do {
        Anim_Remove(q);
        q++;
        i--;
    } while (i >= 0);
    return 0;
}
