#include "palette_set.h"
int PaletteSet_Update(struct PaletteSet *p)
{
    int amount = sub_080036C0(), changed = 0, i;
    struct PaletteSetEntry *palette;
    if (p->current != gUnk_03005430[14]) {
        sub_08227950(&p->from, &p->to);
        goto refresh;
    }
    if (p->blend != amount)
        changed = 1;
    if (changed) {
    refresh:
        p->blend = amount;
        for (i = 0; i < p->count; i++)
            sub_0800E0CC(p, &p->palettes[i]);
    }
    amount = sub_080036DC();
    if (p->overlay != amount) {
        p->overlay = amount;
        for (i = 0; i < p->count; i++)
            sub_0800E10C(p, &p->palettes[i]);
    }
    gUnk_03004DA4 = gUnk_030051CC;
    gUnk_030051C0 = 0;
    for (i = 0; i < p->count; i++) {
        sub_08217344(p->palettes[i].slot, p->palettes[i].colors);
        gUnk_030051C0++;
    }
    p->previous = gUnk_03005430[15];
    p->current = gUnk_03005430[14];
    return 0;
}
