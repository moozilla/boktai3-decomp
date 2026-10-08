#include "global.h"
struct S { u8 f[0x20]; u32 a; u32 b; u8 g[0x88-0x28]; u8 *p[2]; };
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08004048(void *, u32, u32);
void sub_0821A0C0(void *);
void sub_08003C20(void);
void sub_08004018(void);

void *sub_080040F8(u16 x, u16 y)
{
    struct S *s;
    s32 n;
    s32 a;
    s32 b;
    if (Script_SeekToKeyword(0x6e))
        n = Script_GetValue();
    else
        n = 8;
    a = n * 0x84;
    b = n * 8 + 0x90;
    s = sub_08219FBC(9, a + b + n * 12);
    if (s != 0) {
        sub_0821A04C(s, sub_08003C20, sub_08004018);
        s->p[0] = (u8 *)s + b;
        s->p[1] = s->p[0] + a;
        s->a = n;
        s->b = n * 3;
        if (sub_08004048(s, x, y) < 0) {
            sub_0821A0C0(s);
            return 0;
        }
    }
    return s;
}
