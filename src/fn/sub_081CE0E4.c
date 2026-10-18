#include "global.h"
struct SceneNpcMotion {
    u8 unknown0[0x34];
    s16 x, y, z;
};
struct SceneNpc {
    u8 unknown0[0x26];
    s16 variant;
    u8 unknown28[0x5c - 0x28];
    struct SceneNpcMotion motion;
};
void SceneNpc_SetKindMotionVector(struct SceneNpc *p, s32 kind)
{
    s32 height;
    struct SceneNpcMotion *motion;
    switch (kind) {
        case 19:
        case 20:
        case 21:
            height = 240;
            break;
        case 28:
        case 43:
            height = 200;
            break;
        case 29: {
            s32 variant = p->variant;
            height = 400;
            if (variant != 1)
                break;
        }
        case 9:
            height = 528;
            break;
        default:
            height = 400;
            break;
    }
    motion = &p->motion;
    motion->x = 0;
    motion->y = height;
    motion->z = 0;
}
