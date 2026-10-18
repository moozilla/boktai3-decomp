#include "global.h"
#include "scene_pool.h"
#define Glove_GetHorizontalBounds sub_0811AB78
s32 sub_0811AB78(s32 index, s32 *left, s32 *right)
{
    struct ScenePoolPlayer *glove;
    if (index <= 1) {
        glove = gUnk_020004F0[index];
        if (glove) {
            s32 x = glove->position[0];
            x -= ((glove->viewportOriginX >> 12) << 12);
            *left = x;
            *right = *left + (240 << 12);
            return 0;
        } else {
            *left = (s32)glove;
            *right = (s32)glove;
            return -1;
        }
    }
failure:
    *left = 0;
    *right = 0;
    return -1;
}
