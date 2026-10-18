#include "global.h"
struct PairedSceneHalf {
    u8 unknown0[0x20c];
    s16 current;
    s16 reset;
    u8 unknown210[0xc];
};
struct PairedSceneActor {
    struct PairedSceneHalf halves[2];
};
void PairedSceneActor_SetResetValues(struct PairedSceneActor *p, s32 second, s32 first)
{
    struct PairedSceneReset {
        s16 current, reset;
        u8 unknown4[0x218];
    } *half;
    s32 i;
    u32 stride;
    if (p) {
        p->halves[1].reset = second;
        p->halves[0].reset = first;
        half = (struct PairedSceneReset *)&p->halves[0].current;
        stride = 0x21c;
        for (i = 1; i >= 0; i--) {
            if (half->current > 0)
                half->current = half->reset;
            half = (struct PairedSceneReset *)((u8 *)half + stride);
        }
    }
}
