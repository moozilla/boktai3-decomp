#include "global.h"
struct BeeVec {
    s16 x, y, z;
};
#define Bee_SubVector sub_08093644
void *Bee_SubVector(void *unused, struct BeeVec *a, struct BeeVec *b)
{
    struct BeeVec d;
    struct BeeVec *p = &d;
    d.x = b->x - a->x;
    p->y = b->y - a->y;
    p->z = b->z - a->z;
    return unused;
}
