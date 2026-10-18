#include "global.h"
typedef struct {
    u32 pad[2], flags;
    u8 gap[20];
    s16 x, y;
    u8 tail[60];
} Graphic;
void SelectionMenu_SetCallback(void *, void (*)(void));
void SelectionMenu_Choose(void);
void SelectionMenu_ReviewChoice(void);
void sub_0801B600(int, int, void *);
void SelectionMenu_AdvanceChoice(u8 *p)
{
    Graphic *g = (Graphic *)(p + 0x1f4);
    if (!p[0x1e])
        g->x = 64;
    else
        g->x = 104;
    g->y = 40;
    if (++p[0x1e] <= 1)
        SelectionMenu_SetCallback(p, SelectionMenu_Choose);
    else {
        sub_0801B600(p[0x21], p[0x22], p + 0xd94);
        SelectionMenu_SetCallback(p, SelectionMenu_ReviewChoice);
    }
}
