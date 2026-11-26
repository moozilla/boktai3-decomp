#include "global.h"

s32 Div(s32, s32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);

void sub_08243020(u8 *p, s32 amount)
{
    if (*(s32 *)(p + 0x1c) == 1) {
        s32 heal = Div(amount * 5, 100);
        u16 *cur;
        if (heal <= 0)
            heal = 1;
        cur = (u16 *)(p + 0x428);
        if (*cur + heal > *(u16 *)(p + 0x42a)) {
            heal = *(u16 *)(p + 0x42a) - *cur;
            *cur = *(u16 *)(p + 0x42a);
        } else {
            *cur = *cur + heal;
        }
        if (heal > 0)
            DamageNumberDisplay_Show(p + 0x230, heal, 1, p + 0x30, p + 0xae8, p[0xaf0]);
    }
}
