#include "global.h"
#include "scene_stimulus.h"
/* The word return contains a zero-extended 16-bit table tag. */
u32 SceneStimulus_GetTaggedPosition(u16 *o, u32 inputIndex)
{
    u8 i = inputIndex;
    u8 *e = (u8 *)gSceneStimulusState;
    u8 *q;
    u16 a, z;
    if (e == 0)
        return 0;
    q = e + ((s32)((u32)i << 24) >> 20);
    a = *(u16 *)(q + 0x18);
    z = 0;
    o[0] = a;
    o[1] = z;
    o[2] = *(u16 *)(q + 0x1c);
    return *(u16 *)(q + 0x26);
}
