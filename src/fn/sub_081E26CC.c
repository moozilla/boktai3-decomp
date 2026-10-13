#include "global.h"

void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 SoundTest_Initialize(void *);
void sub_0821A0C0(void *);
void SoundTest_Update(void);
void SoundTest_Destroy(void);

void *sub_081E26CC(void)
{
    void *r = sub_08219FBC(0xb, 1572);
    if (r != 0) {
        sub_0821A04C(r, SoundTest_Update, SoundTest_Destroy);
        if (SoundTest_Initialize(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
