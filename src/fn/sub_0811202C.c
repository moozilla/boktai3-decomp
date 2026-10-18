#include "global.h"
struct T {
    u32 lo : 16;
    u32 hi : 16;
    u32 lo2 : 16;
    u32 hi2 : 16;
};
void sub_0811D384(void *);
void sub_0811C9F8(void *, void *, u32);
void sub_0811CB68(void *, void *, void *, s32);
void CharacterModel_SetAnimationFlags(void *, s32);
void CharacterModel_SetComponentRenderMode(void *, s32);
void CharacterModel_InitPalettes(void *, u32);
void StageResults_InitPortraitDisplay(u8 *s)
{
    struct T t;
    u8 *a;
    u8 *b;
    sub_0811D384(s + 0x9B0);
    s[0x11B2] = 0;
    t.lo = 0x30;
    t.hi = 0x50;
    t.lo2 = 0;
    a = s + 0x968;
    b = s + 0xF60;
    sub_0811C9F8(a, b, 0x3D99);
    sub_0811CB68(a, b, &t, 0);
    CharacterModel_SetAnimationFlags(b, 0);
    CharacterModel_SetComponentRenderMode(b, 0);
    CharacterModel_InitPalettes(b, s[0x9B3]);
}
