#include "global.h"
struct PairedSceneHalf {
    u8 unknown0[0x98];
    u8 actor[0x160];
    u32 flags;
    u8 unknown1FC[8];
    u8 mode;
    u8 unknown205[3];
    u8 cleared;
    u8 unknown209[3];
    s16 current;
    u8 unknown20E[0xe];
};
struct PairedSceneActor {
    struct PairedSceneHalf halves[2];
    u8 unknown438[0x17c];
    u32 pending;
};
void Vanargand_ResetState(void *, s32);
static inline u8 IsPositive(struct PairedSceneHalf *h)
{
    u8 result = FALSE;
    if (h->current > 0)
        result = TRUE;
    return result;
}
s32 PairedSceneActor_ApplyToSelectedHalves(struct PairedSceneActor *p, u32 mask, u32 value)
{
    if (!p)
        return 0;
    if (mask & 1) {
        Vanargand_ResetState(p->halves[0].actor, value);
        if (IsPositive(&p->halves[0])) {
            u32 bit = 0x100;
            if (!(p->halves[0].flags & bit)) {
                p->halves[0].mode = value;
                p->halves[0].cleared = 0;
                p->pending |= 1;
            }
        }
    }
    if (mask & 2) {
        Vanargand_ResetState(p->halves[1].actor, value);
        if (IsPositive(&p->halves[1])) {
            u32 bit = 0x100;
            if (!(p->halves[1].flags & bit)) {
                p->halves[1].mode = value;
                p->halves[1].cleared = 0;
                p->pending |= 2;
            }
        }
    }
    return p->pending;
}
