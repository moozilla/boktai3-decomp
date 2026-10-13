#include "global.h"
struct O {
    u8 p[8];
    u16 f8;
    u16 pa;
    u16 fc;
    u16 fe;
    u16 f10;
};
void Collider_SetPosition(struct O *o, u16 *s, u16 v)
{
    o->fc = s[0];
    o->fe = s[1];
    o->f10 = s[2];
    o->f8 = v;
}
