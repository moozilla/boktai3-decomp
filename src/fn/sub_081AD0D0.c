#include "global.h"
#include "vanargand_pair_attack.h"
#include "vanargand_stage_colors.h"
void Sound_PlaySong(u32);
void Vanargand_ResetPaletteTransition(struct VanargandPairAttack *);
void CpuSet(const void *, void *, u32);


static inline void restorePalette(struct VanargandPairAttack *p, struct VanargandPairAttack *self)
{
    s32 kind = 0x6F;
    if (!p->variant)
        kind = 0x7F;
    switch (kind) {
        case 0x6F: {

            CpuSet(gPaletteBankData + 0xDE0, p->palettes, 0x04000020);
            CpuSet(p->fadeA, gBgPaletteBuffer, 0x04000068);
            Vanargand_SetStageColors(p->transition);
            break;
        }
        case 0x82:
            CpuSet(gPaletteBankData + 0x1040, p->palettes, 0x04000020);
            CpuSet(p->fadeB, gBgPaletteBuffer, 0x04000068);
            break;
        case 0x7F:
            CpuSet(gPaletteBankData + 0xFE0, p->palettes, 0x04000018);
            CpuSet(p->fadeC, gBgPaletteBuffer, 0x04000068);
            break;
        default:
            CpuSet(gPaletteBankData + 0x3240, self->palettes, 0x04000008);
            break;
    }
    Vanargand_ResetPaletteTransition(self);
    Vanargand_StopActionSequence(&self->actionSequence);
}
static inline void changeState(struct VanargandPairAttack *p, s32 state,
                               void (*callback)(void *, s32))
{
    struct VanargandPairAttack *self = p;
    // Keep the transition owner distinct from the callback installation owner.
    if (p->state == 3)
        restorePalette(p, self);
    {
        p->state = state;
        p->stateCallback = callback;
        p->callbackFrame = 0;
        p->pending = 1;
        p->phase = 0;
        p->timer = 0;
    }
}


// confidence=low: consumes byte AA and installs its callback from the mode table.
extern void *gUnk_08612020[];
void Vanargand_SetPairedMotionParameter(void *, u16);
s32 Vanargand_ApplyRequestedMode(struct VanargandPairAttack *p)
{
    u8 *request = &p->requestedAnimation;
    if ((u8)(*request - 1) > 31)
        return 0;
    {
        u32 a = 2, b = 4;
        p->attackParameter5B8 = a;
        p->attackParameter5BA = b;
    }
    Vanargand_SetPairedMotionParameter(p->pair, p->parameter584);
    {
        void *callback = gUnk_08612020[*request];
        s32 mode = *request;
        changeState(p, mode, callback);
    }
    return 1;
}
