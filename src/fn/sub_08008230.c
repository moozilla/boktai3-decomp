#include "global.h"

void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 TriggerManager_Initialize(void *, void *);
void sub_0821A0C0(void *);
void TriggerManager_Update(void);
void sub_08008148(void);

void *sub_08008230(void *a)
{
    void *p = sub_08219FBC(9, 0x5c);
    if (p != 0) {
        sub_0821A04C(p, TriggerManager_Update, sub_08008148);
        if (TriggerManager_Initialize(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
