#include "global.h"
struct S { u8 f[0x20]; u32 a; u8 *b; s32 c; };
s32 Script_SeekToKeyword(s32);
u8 *sub_08219C40(s32);
u32 Script_GetValueSafe(void);
void Script_SetPc(void);
s32 Script_GetValue(void);
void sub_08219DD8(void *, s32);
void sub_08219D38(void *);
void sub_08110EDC(struct S *s)
{
    u8 *buf;
    s32 n, i, c, r, m;
    if (Script_SeekToKeyword(0x73)) {
        buf = sub_08219C40(0x1000);
        s->a = Script_GetValueSafe();
        Script_SetPc();
        for (s->c = 0; (c = Script_GetValue()) >= 0; s->c++)
            buf[s->c] = c;
        n = s->c;
        r = 3 & n;
        if (r > 0) {
            r = 4 - r;
            m = n + r;
        } else
            m = n;
        s->b = sub_08219C40(m);
        sub_08219DD8(s->b, m);
        for (i = 0; i < s->c; i++)
            s->b[i] = buf[i];
        sub_08219D38(buf);
    }
}
