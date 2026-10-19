#include "global.h"
s32 Script_GetKeywordValue(u32, s32);
s32 Script_GetPc(void);
s32 Script_GetValue(void);
s32 Script_SeekToKeyword(u32);
void sub_08217060(s32, s32, s32);
void sub_08217074(s32, s32);
void sub_08217084(u32 v);

struct S {
    u8 pad[0x18];
    u8 f18;
    s8 a[2];
    s8 b[2];
    u8 pad2[3];
    u16 f20;
    u16 f22;
    s32 f24;
    s32 f28;
};

u32 sub_08018E00(struct S *p)
{
    s32 v;
    s32 i;
    s32 q;
    u8 *b;
    u8 *a;
    p->f18 = Script_GetKeywordValue(0x6d, 1);
    if (Script_SeekToKeyword(0x69)) {
        a = (u8 *)p->a;
        b = (u8 *)p->b;
        q = (s32)a;
        do {
            v = Script_GetPc();
            if (v)
                v = Script_GetValue();
            *(s8 *)q = v;
            q++;
        } while (q <= (s32)a + 1);
    } else {
        a = (u8 *)p->a;
        b = (u8 *)p->b;
        q = (s32)a + 1;
        do {
            *(s8 *)q = 0;
            q--;
        } while (q >= (s32)a);
    }
    if (Script_SeekToKeyword(0x61)) {
        q = (s32)b;
        for (i = 0; i <= 1; i++) {
            v = Script_GetPc();
            if (v)
                v = Script_GetValue() - a[i];
            *(s8 *)q = v;
            q++;
        }
    } else {
        q = (s32)b + 1;
        do {
            *(s8 *)q = 0;
            q--;
        } while (q >= (s32)b);
    }
    p->f20 = Script_GetKeywordValue(0x73, 0);
    p->f22 = Script_GetKeywordValue(0x64, 0);
    p->f28 = Script_GetKeywordValue(0x74, 0x3c);
    p->f24 = 0;
    if (p->f28 == 0)
        p->f28 = 0x3c;
    sub_08217060(p->f18, p->f20, p->f22);
    if (p->f18 == 1)
        sub_08217074(p->a[0], p->a[1]);
    else
        sub_08217084(p->a[0]);
    return 0;
}
