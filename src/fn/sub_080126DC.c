#include "animated_hazard.h"

s32 sub_08012684(struct AnimatedHazardManager *);
s32 sub_08012630(struct AnimatedHazardManager *);
u32 sub_08012658(struct AnimatedHazardManager *);

void *sub_080126DC(void)
{
    struct AnimatedHazardManager *p;
    if (gUnk_02000050 != 0)
        return gUnk_02000050;
    p = Task_Create(8, 0x538);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08012630, sub_08012658);
        if (sub_08012684(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
