#include "global.h"
#include "ui/books/palette_transition_banks.h"
// Family and descriptive names are tentative interpretations of static code.
struct PaletteTransition {
    u8 pad0[0x1c];
    u32 parameter;
    u8 pad20[8];
    u32 mode;
    u8 pad2c[0x374];
    struct PaletteTransitionBanks banks;
};
void PaletteTransition_StartDisplay(struct PaletteTransition *, u32);
void PaletteTransition_LoadSource(struct PaletteTransition *, u32);
void PaletteTransition_ResetBuffers(struct PaletteTransition *p)
{
    s32 i;

    PaletteTransition_StartDisplay(p, p->parameter);
    if (p->mode == 2) {
        struct PaletteTransitionBanks *banks = &p->banks;

        PaletteTransition_LoadSource(p, p->parameter);
        CpuSet(banks->source, banks->destination, 0x04000080);
        for (i = 0; i < 16; i++)
            banks->target[0][i] = 0x7fff;
        for (i = 0; i < 16; i++)
            banks->target[1][i] = 0;
    }
}
