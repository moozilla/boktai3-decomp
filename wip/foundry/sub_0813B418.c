#define Player_ScaleActionValue sub_0813B418
#include "global.h"
#include "player.h"
s32 Div(s32, s32);
static inline u32 mask(u32 *p, u32 m) { return *p & m; }
s32 sub_0813B418(struct Player *p, s32 value)
{
    if (p && mask(&p->actionFlags, 0x1000) && mask(&p->actionFlags, 0x800))
        return value;
    if (p && mask(&p->actionFlags, 0x1000)) value -= Div(value, 2);
    else if (p && mask(&p->actionFlags, 0x800)) value *= 2;
    return value;
}
