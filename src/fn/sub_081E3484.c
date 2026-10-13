#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 Presentation_Initialize(void *);
void sub_0821A0C0(void *);
void Presentation_Update(void);
void Presentation_Destroy(void);
void *sub_081E3484(void)
{
    void *p = sub_08219FBC(8, 0x238);
    if (p) {
        sub_0821A04C(p, Presentation_Update, Presentation_Destroy);
        if (Presentation_Initialize(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
