#include "global.h"
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void Anim_Step(void *, void *);
void sub_0811CC24(u8 *s, u8 *t)
{
    s32 i = 0;
    do {
        u32 v;
        u8 *a, *b;
        switch (i) {
            case 0:
                v = s[0x48];
                break;
            case 3:
            case 4:
                v = s[0x4a];
                break;
            case 1:
                v = s[0x49];
                break;
            case 2:
                v = s[0x4c];
                break;
            default:
                v = 0;
                break;
        }
        a = t + i * 0x60;
        b = t + 0x1e0;
        Anim_SetKeyframes(a, b, v, 1, 0);
        Anim_Step(a, b);
        i++;
    } while (i <= 4);
}
