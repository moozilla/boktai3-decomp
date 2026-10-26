#include "global.h"
#include "ram.h"
void OrbitEffect_SpawnHomingTrail(void *, s32);

s32 sub_081FED14(s32 index, s32 arg)
{
    s32 result;
    if (gSlotController != NULL) {
        OrbitEffect_SpawnHomingTrail(gSlotController + (index * 0x3ac + 0x38), arg);
        result = 0;
    } else {
        result = -1;
    }
    return result;
}
