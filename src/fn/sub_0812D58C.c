#include "global.h"
#include "ram.h"

struct FragmentActor {
    u8 unknown0[0x1c];
    s16 x, y, z, pad;
    u8 unknown24[0x96 - 0x24];
    u8 state;
    u8 unknown97[8];
    u8 entered;
};

struct FragmentPool {
    u8 unknown0[0x3c];
    struct FragmentActor actors[8];
};

struct PackedVec {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 w : 16;
};

void ScriptedColliderActor_Create(struct PackedVec *position, u32 variant, u32 a, u32 b, u32 c,
                                  u32 d);

s32 sub_0812D58C(s16 *pos)
{
    struct FragmentPool *pool = gScriptedColliderManager;
    s32 i;

    if (pool == NULL)
        return 0;
    for (i = 0; i < 8; i++) {
        if ((u8)(pool->actors[i].state - 1) <= 1 && pool->actors[i].x == pos[0] &&
            pool->actors[i].y == pos[1] && pool->actors[i].z == pos[2])
            return 1;
    }
    return 0;
}

void sub_0812D5F0(void)
{
    struct FragmentPool *pool = gScriptedColliderManager;
    s32 i;

    if (pool != NULL) {
        for (i = 0; i < 8; i++) {
            if (pool->actors[i].state != 3 && pool->actors[i].state != 0) {
                pool->actors[i].state = 3;
                pool->actors[i].entered = 1;
            }
        }
    }
}

void sub_0812D624(void)
{
    struct PackedVec v;
    v.x = 0x980;
    v.y = 0x200;
    v.z = 0x880;
    ScriptedColliderActor_Create(&v, 0x78, 0, 0, 0, 0);
}

void sub_0812D66C(void)
{
}
