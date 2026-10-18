#include "global.h"
/* SceneActors_SetMotionOffsets: Sets two byte fields at offsets 0xB4 and 0xB5; motion interpretation is tentative */
#define SceneActors_SetMotionOffsets sub_080216D8
#define SceneActors_SetMotionOffsets sub_080216D8
int SceneActors_SetMotionOffsets(u8 *p, int a, int b)
{
    p[0xb4] = a;
    p[0xb5] = b;
    return 0;
}
