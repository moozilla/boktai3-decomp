#include "global.h"

s32 Div(s32, s32);
s32 Script_GetValue(void);
s32 Script_SeekToKeyword(u32);
void sub_080214D4(u8 *p, u32 a, u32 b, void *c, u32 d);
void sub_080213EC(u8 *p, u32 i);
u32 sub_0821ABA8(u32 a, u32 b);
void sub_080215AC(u8 *p, u32 a, u32 b, u32 c);
void sub_081F0464(u8 *p, u32 a);
void sub_081F0384(u8 *p);
void sub_081F053C(u8 *p, u32 a);
void sub_081F065C(u8 *p);
void sub_081F0E24(u8 *p);
u32 sub_0821A520(u32 a);
void sub_081F0F68(u8 *p);
u32 sub_081EC380(u8 *p);
void sub_0822B2F8(u32 a);
void *sub_081F2764(void);
void sub_08215284(void *a, u32 b);
void sub_081F0678(void);
extern s32 gFlag020004B8;

u32 sub_081F1C2C(u8 *p, u32 arg1)
{
    s32 i = 0;
    s32 a = 2000;
    u8 *q = p;
    s32 b = 100;
    u32 seven = 7;
    s32 k = 0;
    s32 v = 0;
    s32 f = 0;
    u8 buf[8];
    u32 *r4;
    u16 *s;
    s32 j;

    if (Script_SeekToKeyword(0x65))
        *(s32 *)(p + 0x588) = Script_GetValue();
    if (Script_SeekToKeyword(0x61)) {
        a = Script_GetValue();
        b = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x64)) {
        *(u16 *)(p + 0x5C4) = Script_GetValue();
        *(u16 *)(p + 0x5C6) = Script_GetValue();
        *(u16 *)(p + 0x5CA) = Script_GetValue();
        *(u16 *)(p + 0x5CC) = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x77)) {
        *(u16 *)(p + 0x260) = Script_GetValue();
        *(u16 *)(p + 0x262) = Script_GetValue();
        *(u16 *)(p + 0x264) = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x4d))
        k = Script_GetValue();
    if (Script_SeekToKeyword(0x70)) {
        v = Script_GetValue();
        f = 1;
    }
    if (*(s32 *)0x020004B8 != 0)
        *(u16 *)(p + 0x5EE) = 1;
    else
        *(u16 *)(p + 0x5EE) = i;
    sub_080214D4(q, arg1, seven, buf, i);
    *(u8 *)(q + 0x114) = i;
    sub_080213EC(p, seven);
    *(u8 *)(q + 0xB3) = sub_0821ABA8(0x69, 0);
    *(void **)(q + 0x104) = sub_081F0678;
    sub_080215AC(q, a, a, b);
    *(u16 *)(p + 0x5F0) = Div(a, 7);
    *(u16 *)(p + 0x5F2) = Div(a, 10);
    sub_081F0464(p, b);
    sub_081F0384(p);
    sub_081F053C(p, b);
    sub_081F065C(p);
    sub_081F0E24(p);
    *(u32 *)(p + 0x38C) = 0xEBF6;
    *(u32 *)(p + 0x390) = sub_0821A520(0x922E);
    *(u32 *)(p + 0x388) = 0x2E90;
    sub_081F0F68(p);
    s = (u16 *)(p + 0x3C6);
    for (j = 15; j >= 0; j--)
        *s-- = 0x7FFF;
    sub_081EC380(p);
    *(u32 *)(q + 0x9C) |= 0x10;
    if (*(u8 *)(q + 0xB3) == 0) {
        *(u8 *)(q + 0xAC) = 5;
        *(u8 *)(q + 0xAD) = 0;
        *(u8 *)(q + 0xAE) = 5;
        *(u32 *)(q + 0xC4) = 0;
        *(u8 *)(q + 0xB0) = 1;
    } else {
        if (f) {
            *(u16 *)(p + 0x278) = v;
            *(s32 *)(p + 0x294) = *(s16 *)(p + 0x278);
            *(u16 *)(p + 0x5D2) = 1;
            *(u8 *)(q + 0xAC) = 1;
            *(u8 *)(q + 0xAD) = 0;
            *(u8 *)(q + 0xAE) = 2;
            *(u32 *)(q + 0xC4) = 0;
            *(u8 *)(q + 0xB0) = 2;
        } else {
            *(u16 *)(p + 0x278) = f;
            *(u8 *)(q + 0xAC) = 1;
            *(u8 *)(q + 0xAD) = 158;
            *(u8 *)(q + 0xAE) = 1;
            *(u32 *)(q + 0xC4) = f;
            *(u8 *)(q + 0xB0) = 1;
        }
        if (*(s32 *)0x020004B8 != 0)
            sub_0822B2F8(0x586);
    }
    *(u16 *)(p + 0x5D0) = 0;
    *(void **)(p + 0x62C) = sub_081F2764();
    *(u32 *)(p + 0x5AC) = 7;
    *(u32 *)(p + 0x5A8) = 0x80;
    if (*(u8 *)(q + 0xB3) != 0 && k > 0) {
        CpuSet((u8 *)(p + 0x488), (void *)0x03005040, 0x40);
        sub_08215284(p + 0x350, 0x1F6);
        CpuSet(*(void **)(p + 0x35C), p + 0x3C8, 0x10);
        sub_08215284(p + 0x36C, 0x1F5);
        CpuSet(*(void **)(p + 0x378), p + 0x3E8, 0x10);
        *(void **)(p + 0x35C) = p + 0x3C8;
        *(void **)(p + 0x378) = p + 0x3E8;
        *(u32 *)(p + 0x5FC) = 0x1F6;
        *(u32 *)(p + 0x608) = 0x1F5;
        *(u32 *)(p + 0x304) = 1;
        *(u32 *)(p + 0x34C) = 1;
        if (k == 1) {
            *(u32 *)(p + 0x5A0) = 0x3C;
            *(u16 *)(p + 0x5E4) = 1;
        } else {
            *(u16 *)(p + 0x5E4) = 0;
        }
    } else {
        {
            u8 *src = (u8 *)0x03004FC0 + 0x80;
            u8 *dst = p + 0x508;
            for (j = 20; j >= 158; j--) {
                CpuSet(src, dst, 0x10);
                dst += 0x20;
                src += 0x20;
            }
        }
        *(u32 *)(p + 0x5FC) = 2;
        *(u32 *)(p + 0x608) = 1;
    }
    *(u16 *)(p + 0x5E2) = 0;
}
