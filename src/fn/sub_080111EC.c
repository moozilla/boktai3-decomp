#include "global.h"
struct Menu {
    u8 pad[0x450];
    void *map, *palette;
};
void *sub_0821A520(int, int);
void sub_082161B4(int, int, void *, int, int, int, void *);
void sub_0821656C(int, int, int, int, int);
extern u16 gUnk_03004FC0[];
#define MessageMenu_LoadBackground sub_080111EC
void MessageMenu_LoadBackground(struct Menu *m, int id)
{
    void *map = sub_0821A520(0xC091, 0x3536);
    m->map = map;
    sub_082161B4(1, 0, map, 0, 0, 1, &id);
    sub_0821656C(2, 0, 0, 0, 0);
    m->palette = (u8 *)sub_0821A520(0x92B3, 0x204) + 20;
    CpuSet(m->palette, gUnk_03004FC0, 256);
}
