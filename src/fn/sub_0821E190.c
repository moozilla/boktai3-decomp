#include "global.h"
struct Trap { u32 serial; u16 f4, f6, id, fa, flags, fe; u16 w[4], s[4]; u32 f20, command; void *f28; };
struct Context { u16 count; u8 pad[0x222]; struct Trap traps[64]; u8 *tops[64]; };
extern struct Context *gUnk_030052F4;
extern u32 gUnk_03005300;
void sub_0821E190(struct Trap *trap, u8 *top)
{
    s32 i, found, j;
    if (gUnk_030052F4->count > 63) return;
    if (!gUnk_03005300) gUnk_03005300 = 1;
    trap->serial = gUnk_03005300++;
    found = 0;
    i = gUnk_030052F4->count;
    while (i > 0) {
        if (trap->id == gUnk_030052F4->traps[i - 1].id) { found = 1; break; }
        i--;
    }
    if (!found) {
        gUnk_030052F4->traps[gUnk_030052F4->count] = *trap;
        gUnk_030052F4->tops[gUnk_030052F4->count] = top;
    } else {
        for (j = gUnk_030052F4->count; j > i; j--) {
            gUnk_030052F4->traps[j] = gUnk_030052F4->traps[j - 1];
            gUnk_030052F4->tops[j] = gUnk_030052F4->tops[j - 1];
        }
        gUnk_030052F4->traps[i] = *trap;
        gUnk_030052F4->tops[i] = top;
    }
    gUnk_030052F4->count++;
}
