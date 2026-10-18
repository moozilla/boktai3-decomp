#include "global.h"
struct SceneEffect {
    u8 unknown0[0x174];
    u32 unknown174;
    u32 unknown178[2];
    u8 unknown180;
    u8 unknown181;
    u16 unknown182;
    u32 unknown184;
};
static inline void clear(void *dst, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, dst, 0x05000002);
}
void SceneEffect_ResetState(struct SceneEffect *effect)
{
    u32 zero;
    effect->unknown174 = 0;
    clear(effect->unknown178, &zero);
    effect->unknown180 = 0;
    effect->unknown181 = 0;
    effect->unknown182 = 0;
    effect->unknown184 = 0;
}
