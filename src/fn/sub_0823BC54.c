#include "global.h"

/* tentative: probes tiles beside the facing and picks a diagonal direction */
#define Actor_ProbeDiagonalTurnDir sub_0823BC54

struct Vec16 {
    s16 x;
    s16 y;
    s16 z;
    s16 w;
};

struct Obj {
    u8 pad0[0x30];
    struct Vec16 pos;
    u8 pad38[0x48F - 0x38];
    u8 dir;
};

s32 sub_0823BB48(struct Obj *obj, struct Vec16 *v);

s32 Actor_ProbeDiagonalTurnDir(struct Obj *obj)
{
    struct Vec16 v;

    switch (obj->dir) {
        case 0:
            v = obj->pos;
            v.x -= 30;
            v.z -= 30;
            v.x -= 100;
            if (sub_0823BB48(obj, &v))
                return 1;
            v.x += 100;
            v.z -= 100;
            if (sub_0823BB48(obj, &v))
                return 7;
            v = obj->pos;
            v.x -= 100;
            if (sub_0823BB48(obj, &v))
                return 1;
            return 7;
        case 2:
            v = obj->pos;
            v.x += 30;
            v.z -= 30;
            v.x += 100;
            if (sub_0823BB48(obj, &v))
                return 1;
            v.x -= 100;
            v.z -= 100;
            if (sub_0823BB48(obj, &v))
                return 3;
            v = obj->pos;
            v.x += 100;
            if (sub_0823BB48(obj, &v))
                return 1;
            return 3;
        case 4:
            v = obj->pos;
            v.x += 30;
            v.z += 30;
            v.x += 100;
            if (sub_0823BB48(obj, &v))
                return 5;
            v.x -= 100;
            v.z += 100;
            if (sub_0823BB48(obj, &v))
                return 3;
            v = obj->pos;
            v.x += 100;
            if (sub_0823BB48(obj, &v))
                return 5;
            return 3;
        case 6:
            v = obj->pos;
            v.x -= 30;
            v.z += 30;
            v.x -= 100;
            if (sub_0823BB48(obj, &v))
                return 5;
            v.x += 100;
            v.z += 100;
            if (sub_0823BB48(obj, &v))
                return 7;
            v = obj->pos;
            v.x -= 100;
            if (sub_0823BB48(obj, &v))
                return 5;
            return 7;
    }
    return -1;
}
