#include "global.h"
struct O {
    u8 p[0x48];
    u8 f48;
};
void Collider_SetContactMode(struct O *o, u8 a)
{
    o->f48 = a;
}
