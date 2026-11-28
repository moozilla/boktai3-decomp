#include "global.h"
#include "ui/books/bestiary_display.h"

void DisplayAnimation_Destroy(void *);
s32 PaletteTransition_StartDisplay(struct BestiaryDisplay *p, int index);

void PaletteTransition_Restart(struct BestiaryDisplay *p)
{
    const struct BestiaryDisplayConfig *config = &gUnk_086126E0[p->entry];
    s32 i;

    p->config = config;
    for (i = 0; i < 6; i++) {
        if (p->animations[i].active)
            DisplayAnimation_Destroy(&p->animations[i]);
    }
    p->sequence = config->sequence;
    if (p->mode == 0) {
        s32 x = config->x;
        s32 y = config->y;
        u16 *origin = p->origin;

        x += 0x40;
        y += 0x54;
        origin[0] = x;
        origin[1] = y;
        origin[2] = 0;
        origin[3] = 0;
    }
    if (!config->create)
        PaletteTransition_StartDisplay(p, p->entry);
    else
        config->create(p);
}
