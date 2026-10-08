#include "global.h"
struct S { u8 p1[0x124]; s16 n; s16 pad; s16 d[8]; u8 p0[0x15c - 0x138]; u8 *pc; };
void Script_SetPc(u8 *);
u8 *Script_GetPc(void);
s32 Script_GetValue(void);
void sub_08205A6C(struct S *p)
{
    s32 i;
    s16 *d;
    s16 *np;
    Script_SetPc(p->pc);
    i = 0;
    if (Script_GetPc() != 0) {
        s32 v = Script_GetValue();
        np = &p->n;
        *np = v;
        if (*np > 8)
            *np = 8;
        if (*np != 0 && i < *np) {
            d = p->d;
            do {
                *d = Script_GetValue();
                d++;
                i++;
            } while (i < p->n);
        }
    }
}
