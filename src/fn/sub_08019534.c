#include "global.h"
typedef struct {
    u8 pad[24];
    int time, frame;
    u16 colors[256];
} PaletteFrames;
extern u16 gUnk_03004FC0[];
int Palette_IsDirty(void);
void PaletteFrames_CaptureColors(PaletteFrames *);
int PaletteFrames_Update(PaletteFrames *p)
{
    int i;
    if (Palette_IsDirty())
        PaletteFrames_CaptureColors(p);
    if (++p->time > 3) {
        p->time = 0;
        if (++p->frame > 6)
            p->frame = 0;
        for (i = 0; i < 16; i++)
            gUnk_03004FC0[i] = p->colors[p->frame * 16 + i];
    }
    return 0;
}
