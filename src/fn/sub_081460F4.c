#include "global.h"

s32 sub_0813AE18(u8 *, s32, s32, s32);
void sub_0813B03C(u8 *p, u8 v);
void sub_0813B134(u8 *, s32);
s32 sub_081419D4(u32, s32);
u8 sub_082212DC(void *, u32, s32, s32);
void sub_082279A8(s32, s32, s32, s32, s32, s32, s32);
void Sound_PlaySong(s32);
extern u32 gUnk_030053F4;

#define F8(o)  (*(u8 *)(p + (o)))
#define F16(o) (*(u16 *)(p + (o)))

void sub_081460F4(u8 *p)
{
    sub_0813B134(p, 4);
    switch (F8(0x458)) {
        case 0: {
            u32 z;
            u8 *q;
            sub_0813AE18(p, sub_081419D4(F8(0x418), 0x29), 0x40, 1);
            Sound_PlaySong(0x1AD);
            q = p + 0x153;
            z = 0;
            *q = 1;
            sub_0813B03C(p, 1);
            F16(0x4E8) = z;
            *(u32 *)(p + 0x40) = z;
            break;
        }
        case 1:
            F16(0x45A) = F16(0x45A) + 1;
            if (F16(0x45A) > 0xe) {
                sub_082279A8(3, 5, 4, 4, 4, 0x1FFF, 2);
                F16(0x4E8) = 0;
                sub_0813B03C(p, 2);
            }
            break;
        case 2:
            if (F16(0x45A) == 9) {
                if (F8(0x418) == 6)
                    Sound_PlaySong(0x2EC);
                else
                    Sound_PlaySong(0x304);
            }
            F16(0x45A) = F16(0x45A) + 1;
            if (F16(0x45A) > 0xe) {
                sub_0813B03C(p, 3);
                F16(0x4E8) = 0;
                Sound_PlaySong(0x1AF);
            }
            break;
        case 3:
            sub_0813AE18(p, sub_081419D4(F8(0x418), 0x12), 0x40, 1);
            F16(0x4E8) = F16(0x4E8) - 0x1E;
            F16(0x45A) = F16(0x45A) + 1;
            if (F16(0x45A) > 0x27) {
                F16(0x424) = 0;
                sub_0813B03C(p, 4);
            } else {
                sub_082212DC(p + 0x14C, F16(0x45A), 0xA, 0x14);
            }
            break;
        case 4:
            F16(0x4E8) = 0;
            sub_0813B134(p, 1);
            F16(0x45A) = F16(0x45A) + 1;
            if (F16(0x45A) > 0x3b) {
                sub_0813B03C(p, 5);
                gUnk_030053F4 |= 0x400;
            }
            break;
        case 5:
            sub_0813B134(p, 1);
            break;
    }
}
