#include "global.h"

#define U8(p, o) (*(u8 *)((p) + (o)))
#define U16(p, o) (*(u16 *)((p) + (o)))
#define S16(p, o) (*(s16 *)((p) + (o)))
#define U32(p, o) (*(u32 *)((p) + (o)))

s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
s32 Script_GetKeywordValue(u32, s32);
int SceneActors_Initialize(u8 *p, int id, int type, int arg, int facingArg);
int SceneActors_SetHealthLimits(u8 *p, int current, int maximum, int extra);
void sub_080213EC(u32 v, u32 i);
s32 Div(s32, s32);
void *ScriptProc_Start();
void Sound_PlaySong(u32 song);
void CpuSet(const void *src, void *dest, u32 control);
void AnimData_SelectPaletteBank(void *, u32 animation);
u32 sub_081EC380(void);
void sub_081F065C(u8 *p);
void sub_081F0E24(u8 *s);
void *sub_081F2764(void);
void sub_081F0678(void);
void sub_081F0384();
void sub_081F0464();
void sub_081F053C();
void sub_081F0F68();
extern s32 gUnk_020004B8;

void sub_081F1C2C(u8 *p, int id)
{
    int z = 0;
    u8 *q = p;
    int hp = 2000;
    int max = 100;
    int type = 7;
    int a = 0;
    int b = 0;
    int c = 0;
    u8 buf[8];
    u8 *b3;
    u8 *f9c, *fac, *fad, *fae, *fc4, *fb0;
    u16 *pp;

    if (Script_SeekToKeyword(0x65))
        U32(p, 0x588) = Script_GetValue();
    if (Script_SeekToKeyword(0x61)) {
        hp = Script_GetValue();
        max = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x64)) {
        U16(p, 0x5C4) = Script_GetValue();
        U16(p, 0x5C6) = Script_GetValue();
        U16(p, 0x5CA) = Script_GetValue();
        U16(p, 0x5CC) = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x77)) {
        U16(p, 0x260) = Script_GetValue();
        U16(p, 0x262) = Script_GetValue();
        U16(p, 0x264) = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x4D))
        a = Script_GetValue();
    if (Script_SeekToKeyword(0x70)) {
        b = Script_GetValue();
        c = 1;
    }
    if (gUnk_020004B8)
        U16(p, 0x5EE) = 1;
    else
        U16(p, 0x5EE) = z;
    SceneActors_Initialize(q, id, type, (int)buf, z);
    U8(q, 0x114) = z;
    sub_080213EC((u32)p, type);
    *(b3 = q + 0xB3) = Script_GetKeywordValue(0x69, 0);
    U32(q, 0x104) = (u32)sub_081F0678;
    SceneActors_SetHealthLimits(q, hp, hp, max);
    U16(p, 0x5F0) = Div(hp, 7);
    U16(p, 0x5F2) = Div(hp, 10);
    sub_081F0464(p, max);
    sub_081F0384(p);
    sub_081F053C(p, max);
    sub_081F065C(p);
    sub_081F0E24(p);
    U32(p, 0x38C) = 0xEBF6;
    U32(p, 0x390) = (u32)ScriptProc_Start(0x922E);
    U32(p, 0x388) = 0x2E90;
    sub_081F0F68(p);
    f9c = b3 - 0x17;
    fac = q + 0xAC;
    fad = q + 0xAD;
    fae = q + 0xAE;
    fc4 = q + 0xC4;
    fb0 = q + 0xB0;
    pp = (u16 *)(p + 0x3C6);
    {
    int i = 15;
    do {
        *pp-- = 0x7FFF;
    } while (--i >= 0);
    }
    sub_081EC380();
    *(u32 *)f9c |= 0x10;
    if (*b3 == 0) {
        *fac = 0;
        *fad = 0;
        *fae = 5;
        *(u32 *)fc4 = 0;
        *fb0 = 1;
    } else {
        if (c) {
            U16(p, 0x278) = b;
            U32(p, 0x294) = S16(p, 0x278);
            U16(p, 0x5D2) = 1;
            *fac = 1;
            *fad = 0;
            *fae = 3;
            *(u32 *)fc4 = 0;
            *fb0 = 1;
        } else {
            U16(p, 0x278) = c;
            *fac = 1;
            *fad = 0;
            *fae = 2;
            *(u32 *)fc4 = c;
            *fb0 = 1;
        }
        if (gUnk_020004B8)
            Sound_PlaySong(0x586);
    }
    U16(p, 0x5D0) = 0;
    U32(p, 0x62C) = (u32)sub_081F2764();
    U32(p, 0x5AC) = 7;
    U32(p, 0x5A8) = 0x80;
    if (*b3 != 0 && a > 0) {
        CpuSet((void *)0x03005040, p + 0x488, 0x40);
        AnimData_SelectPaletteBank(p + 0x350, 0x1F4);
        CpuSet(*(void **)(p + 0x35C), p + 0x3C8, 0x10);
        AnimData_SelectPaletteBank(p + 0x36C, 0x1F5);
        CpuSet(*(void **)(p + 0x378), p + 0x3E8, 0x10);
        U32(p, 0x35C) = (u32)(p + 0x3C8);
        U32(p, 0x378) = (u32)(p + 0x3E8);
        U32(p, 0x5FC) = 0x1F4;
        U32(p, 0x60C) = 0x1F5;
        U32(p, 0x304) = 1;
        U32(p, 0x34C) = 1;
        if (a == 2) {
            U32(p, 0x5A0) = 0x3C;
            U16(p, 0x5E4) = 1;
        } else {
            U16(p, 0x5E4) = 0;
        }
    } else {
        u8 *s = (u8 *)0x03004FC0 + 0x80;
        u8 *d = p + 0x508;
        int i;
        for (i = 3; i >= 0; i--) {
            CpuSet(s, d, 0x10);
            d += 0x20;
            s += 0x20;
        }
        U32(p, 0x5FC) = 3;
        U32(p, 0x60C) = 2;
    }
    U16(p, 0x5E2) = 0;
}
