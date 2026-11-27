#include "global.h"
void Anim_Remove(void *);
void Effect_Remove(void *);
int ResultMenu_DestroyGraphics(u8 *p)
{
    u8 *g = p + 0x144;
    int i;
    for (i = 14; i >= 0; g += 96, i--)
        Anim_Remove(g);
    g = p + 0x6e4;
    for (i = 3; i >= 0; g += 40, i--)
        Effect_Remove(g);
}
