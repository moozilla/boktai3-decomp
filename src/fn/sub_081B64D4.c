#define PairedSceneActor_AreSelectedHalvesFinished sub_081B64D4
#include "global.h"
struct PairedSceneHalf {
    u8 unknown0[0x160];
    u32 flags;
    u8 unknown164[8];
    u8 requestedState, state;
    u8 unknown16E[2];
    u8 finished;
    u8 unknown171[3];
    s16 current;
    u8 unknown176[0xA6];
};
struct PairedSceneActor {
    u8 unknown0[0x98];
    struct PairedSceneHalf halves[2];
    u8 unknown4D0[0xE4];
    u32 selectedMask;
};
static inline u8 IsPositive(struct PairedSceneHalf *half)
{
    u8 result = 0;
    if (half->current > 0)
        result = 1;
    return result;
}
u32 sub_081B64D4(struct PairedSceneActor *p)
{
    if (p->selectedMask & 1) {
        if (IsPositive(&p->halves[0])) {
            if (!p->halves[0].finished)
                return 0;
        }
    }
    if (p->selectedMask & 2) {
        if (IsPositive(&p->halves[1])) {
            if (!p->halves[1].finished)
                return 0;
        }
    }
    return 1;
}
