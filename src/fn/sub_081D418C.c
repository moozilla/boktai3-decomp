#include "global.h"
// Family and descriptive names are tentative interpretations of static code.
struct PaletteTransition {
    u8 pad0[0x1c];
    u32 parameter;
    u8 pad20[8];
    u32 mode;
    u8 pad2c[0x374];
    u16 source[0x100];
    u16 destination[0x100];
    u16 white[16];
    u16 black[16];
};
void sub_081D48D8(struct PaletteTransition *, u32);
void sub_081D409C(struct PaletteTransition *, u32);
void PaletteTransition_ResetBuffers(struct PaletteTransition *p)
{
    u16 *source;
    s32 i;
    u16 value;
    sub_081D48D8(p, p->parameter);
    if (p->mode == 2) {
        source = p->source;
        sub_081D409C(p, p->parameter);
        CpuSet(source, p->destination, 0x04000080);
        value = 0x7fff;
        for (i = 15; i >= 0; i--)
            p->white[i] = value;
        value = 0;
        for (i = 15; i >= 0; i--)
            ((u16 *)((u8 *)source + 0x420))[i] = value;
    }
}
