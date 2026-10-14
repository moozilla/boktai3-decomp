#include "global.h"
// Tentative: Turn toward the selected target by at most four angular units.
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
void sub_0802C8CC(struct Object *, int);
void sub_0802CC94(struct Object *p)
{
    if (p->target >= 0) {
        s8 *deltaPtr = (s8 *)(&p->delta);
        int delta;
        int angle;
        ;
        if ((*deltaPtr) > 3) {
            angle = p->angle + 4;
        } else if ((*deltaPtr) <= (-4)) {
            angle = p->angle - 4;
        } else {
            angle = (*deltaPtr) + p->angle;
        }
        sub_0802C8CC(p, angle);
    }
}
