#include "global.h"

void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 EffectManager_Init(void *, void *);
void sub_0821A0C0(void *);
void EffectManager_UpdateAll(void);
void sub_0800AB18(void);

void *sub_0800AC00(void *a)
{
    void *p = sub_08219FBC(8, 0x7DC);
    if (p != 0) {
        sub_0821A04C(p, EffectManager_UpdateAll, sub_0800AB18);
        if (EffectManager_Init(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
