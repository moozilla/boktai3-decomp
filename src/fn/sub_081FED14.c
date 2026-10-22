#include "global.h"
extern u8 *gUnk_020005EC;
void OrbitEffect_SpawnHomingTrail(void *, s32);

s32 sub_081FED14(s32 index, s32 arg)
{
    s32 result;
    if (gUnk_020005EC != NULL) {
        OrbitEffect_SpawnHomingTrail(gUnk_020005EC + (index * 0x3ac + 0x38), arg);
        result = 0;
    } else {
        result = -1;
    }
    return result;
}
