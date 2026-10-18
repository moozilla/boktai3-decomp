#include "global.h"
struct S {
    u8 f[0x1c];
    u8 a;
    u8 g[0xb];
    u32 b;
};
void SelectionMenu_SetCallback(u8 *p, u32 v)
{
    *(u32 *)(p + 0x28) = 0;
    p[0x1c] = 1;
    *(u32 *)(p + 0xda4) = v;
}
