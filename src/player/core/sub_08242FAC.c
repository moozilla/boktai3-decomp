#include "global.h"

s32 Div(s32, s32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);

void sub_08242FAC(u8 *p, s32 amount)
{
    if (*(s32 *)(p + 0x1c) == 1) {
        s32 heal = Div(amount * 5 * 4, 100);
        u16 *cur;
        if (heal <= 0)
            heal = 1;
        cur = (u16 *)(p + 0x424);
        if (*cur + heal > *(u16 *)(p + 0x426)) {
            heal = *(u16 *)(p + 0x426) - *cur;
            *cur = *(u16 *)(p + 0x426);
        } else {
            *cur = *cur + heal;
        }
        if (heal > 0)
            DamageNumberDisplay_Show(p + 0x230, heal, 1, p + 0x30, p + 0xae8, p[0xaf0]);
    }
}
