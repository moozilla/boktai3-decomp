#include "global.h"

struct PairedSceneHalf {
    u8 unk_00[0x160];
    u32 flags;
    u8 unk_164[8];
    u8 mode;
    u8 unk_16D[3];
    u8 cleared;
    u8 unk_171[3];
    s16 current;
    u8 unk_176[0xA6];
};
struct PairedSceneActor {
    u8 unk_00[0x98];
    struct PairedSceneHalf halves[2];
};

void Vanargand_ResetState(void *, s32);

static inline u8 IsPositive(struct PairedSceneHalf *half)
{
    u8 result = FALSE;
    if (half->current > 0)
        result = TRUE;
    return result;
}

static inline void ApplyState(struct PairedSceneHalf *half, u32 value)
{
    u32 bit = 0x100;
    if (!(half->flags & bit)) {
        half->mode = value;
        half->cleared = 0;
    }
}

void PairedSceneActor_ApplyToHalf(struct PairedSceneActor *p, u32 index, u32 value)
{
    if (p && index < 2) {
        Vanargand_ResetState(&p->halves[index], value);
        if (IsPositive(&p->halves[index])) {
            ApplyState(&p->halves[index], value);
        }
    }
}
