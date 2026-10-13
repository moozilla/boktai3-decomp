#include "global.h"
extern u32 gUnk_02000034;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 Breakable_InitManager(void *);
void sub_0821A0C0(void *);
void Breakable_UpdateAll(void);
void Breakable_DestroyManager(void);

void *sub_08009180(void)
{
    void *p;
    if (gUnk_02000034 != 0)
        return (void *)gUnk_02000034;
    p = sub_08219FBC(10, 0x80);
    if (p != 0) {
        sub_0821A04C(p, Breakable_UpdateAll, Breakable_DestroyManager);
        if (Breakable_InitManager(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
