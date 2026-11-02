#include "global.h"
#include "render.h"
#include "vanargand_stage_colors.h"
struct VanargandPart {
    u8 pad0[0x10];
    u16 state;
};
struct Vanargand {
    u8 pad0[0x54];
    u8 motionData[0x9C - 0x54];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[0xAF - 0xA8];
    u8 finished, entered, pendingAnimation, ready;
    u8 padB3[0x18C - 0xB3];
    u8 animation[0x60];
    u8 drawable[0x3FC - 0x1EC];
    struct VanargandPart part;
    u8 pad410[0x500 - 0x410];
    u16 partActive;
    u8 pad502[0x534 - 0x502];
    u8 track[0x29];
    u8 trackState;
    u8 pad55E[0x56C - 0x55E];
    void *attachment;
    u8 pad570[0x5B0 - 0x570];
    u16 delay;
    u8 pad5B2[0x5B8 - 0x5B2];
    u16 field5B8, field5BA;
    u8 pad5BC[0x718 - 0x5BC];
    u8 palette[0x918 - 0x718];
    u8 fadeA[0xB18 - 0x918];
    u8 fadeB[0xD18 - 0xB18];
    u8 fadeC[0xF24 - 0xD18];
    u32 variant;
    u8 padF28[0xF34 - 0xF28];
    s32 transition;
    u8 padF38[0xF44 - 0xF38];
    void *afterimages;
};
void Sound_PlaySong(u32);
void Vanargand_ResetPaletteTransition(struct Vanargand *);
void CpuSet(const void *, void *, u32);

#define FIELD(type, p, offset) (*(type *)((u8 *)(p) + (offset)))
void Vanargand_StopActionSequence(void *);

static inline void restorePalette(struct Vanargand *p, struct Vanargand *self)
{
    s32 kind = 0x6F;
    if (!p->variant)
        kind = 0x7F;
    switch (kind) {
        case 0x6F: {

            CpuSet(gPaletteBankData + 0xDE0, p->palette, 0x04000020);
            CpuSet(p->fadeA, gBgPaletteBuffer, 0x04000068);
            Vanargand_SetStageColors(p->transition);
            break;
        }
        case 0x82:
            CpuSet(gPaletteBankData + 0x1040, p->palette, 0x04000020);
            CpuSet(p->fadeB, gBgPaletteBuffer, 0x04000068);
            break;
        case 0x7F:
            CpuSet(gPaletteBankData + 0xFE0, p->palette, 0x04000018);
            CpuSet(p->fadeC, gBgPaletteBuffer, 0x04000068);
            break;
        default:
            CpuSet(gPaletteBankData + 0x3240, self->palette, 0x04000008);
            break;
    }
    Vanargand_ResetPaletteTransition(self);
    Vanargand_StopActionSequence((u8 *)self + 0x21C);
}
static inline void changeState(struct Vanargand *p, s32 state, void *callback)
{
    struct Vanargand *self = p;
    // Keep the transition owner distinct from the callback installation owner.
    if (FIELD(u8, p, 0xAB) == 3)
        restorePalette(p, self);
    {
        u32 zero = 0;
        FIELD(u8, p, 0xAB) = state;
        FIELD(void *, p, 0x100) = callback;
        FIELD(u32, p, 0x90) = zero;
        p->pendingAnimation = 1;
        p->phase = zero;
        p->timer = zero;
    }
}

// confidence=low: consumes byte AA and installs its callback from the mode table.
extern void *gUnk_08612020[];
void Vanargand_SetPairedMotionParameter(void *, u16);
s32 Vanargand_ApplyRequestedMode(struct Vanargand *p)
{
    u8 *request = (u8 *)p + 0xAA;
    if ((u8)(*request - 1) > 31)
        return 0;
    {
        u32 a = 2, b = 4;
        p->field5B8 = a;
        p->field5BA = b;
    }
    Vanargand_SetPairedMotionParameter(p->attachment, FIELD(u16, p, 0x584));
    {
        void *callback = gUnk_08612020[*request];
        s32 mode = *request;
        changeState(p, mode, callback);
    }
    return 1;
}
