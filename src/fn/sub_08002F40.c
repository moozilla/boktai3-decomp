#include "palette_controller.h"
// MATCHING_CONCESSION: register-pinning
void Palette_BlendRange(u16 *, const u16 *, const u16 *, u32, s32, s32);
void Palette_BlendVariants(struct PaletteController *p)
{
    s32 i;
    register u16 *out asm("r5");
    if (p->blend == 0) {
        i = 0;
        out = p->palettes[0];
        do {
            if (p->source[i])
                CpuSet(p->source[i], out, 0x04000080);
            out += 256;
            i++;
        } while (i <= 3);
    } else if (p->blend == 64) {
        i = 0;
        out = p->palettes[0];
        do {
            if (p->destination[i])
                CpuSet(p->destination[i], out, 0x04000080);
            out += 256;
            i++;
        } while (i <= 3);
    } else {
        i = 0;
        out = p->palettes[0];
        do {
            if (p->source[i] && p->destination[i])
                Palette_BlendRange(out, p->source[i], p->destination[i], p->blend, 0, 208);
            out += 256;
            i++;
        } while (i <= 3);
    }
    p->current = p->palettes[p->selected];
    p->dirty = 1;
}
