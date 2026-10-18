#include "global.h"
struct P {
    u8 p[8];
    u8 *f8;
};
u32 Route_GetCurrentPointNodeId(struct P *p)
{
    return p->f8[4];
}
