#include "global.h"
// MATCHING_CONCESSION: register-pinning

#ifdef NONMATCHING
// fuzzy: 89.5%
// fix.py best (sweep after the inlined-insn fix)
/* Remaining differences: script/vector stack layout and state/resource setup lifetimes. */
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
s32 Script_GetKeywordValue(u32, s32);
s32 SceneActors_Initialize(u8 *, s32, s32, void *, s32);
void SceneActors_Register(u8 *, u32);
s32 SceneActors_SetHealthLimits(u8 *, s32, s32, s32);
s32 Div(s32, s32);
void SingletonSlot7_InitSecondaryCollider(u8 *, s32);
void SingletonSlot7_InitCollider(u8 *);
void SingletonSlot7_InitContactCollider(u8 *, s32);
void SingletonSlot7_RegisterRenderCallback(u8 *);
void SingletonSlot7_InitBackgroundAndMotion(u8 *);
void *ScriptProc_Start(u16, u16);
void SceneActors_InitResources(u8 *);
void Sound_PlaySong(u32);
void *SingletonSlot7_CreateEffectPool(void);
void AnimData_SelectPaletteBank(void *, u32);
s32 sub_081F0678(void);
void SingletonSlot7_InitDisplaySprites(u8 *) __asm__("SingletonSlot7_InitDisplaySprites");
extern u32 gUnk_020004B8;
extern u8 gUnk_03004FC0[], gUnk_03005040[];
void CpuSet(const void *, void *, u32);

#define F(ty, off) (*(ty *)(p + (off)))

s32 SingletonSlot7_InitFromScript(u8 *p, s32 id)
{
    register s32 health asm("r6") = 2000;
    s32 extra = 100;
    register s32 zero asm("r4") = 0;
    s32 type = 7;
    register u8 *q asm("r5") = p;
    s32 mode = 0;
    s32 shared = 0;
    s32 hasShared = 0;
    struct {
        s16 x;
        s16 y;
        s16 z;
        s16 w;
    } vec;
    register u8 *b3 asm("r4");
    u8 *modeField;
    if (Script_SeekToKeyword(0x65))
        *((s32 *)(p + 0x588)) = Script_GetValue();
    if (Script_SeekToKeyword(0x61)) {
        health = Script_GetValue();
        extra = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x64)) {
        *((s16 *)(p + 0x5C4)) = Script_GetValue();
        *((s16 *)(p + 0x5C6)) = Script_GetValue();
        *((s16 *)(p + 0x5CA)) = Script_GetValue();
        *((s16 *)(p + 0x5CC)) = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x77)) {
        *((s16 *)(p + 0x260)) = Script_GetValue();
        *((s16 *)(p + 0x262)) = Script_GetValue();
        *((s16 *)(p + 0x264)) = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x4d))
        mode = Script_GetValue();
    if (Script_SeekToKeyword(0x70)) {
        shared = Script_GetValue();
        hasShared = 1;
    }
    if (gUnk_020004B8)
        *((u16 *)(p + 0x5EE)) = 1;
    else
        *((u16 *)(p + 0x5EE)) = zero;
    SceneActors_Initialize(q, id, type, &vec, zero);
    *((u8 *)(q + 0x114)) = zero;
    SceneActors_Register(p, type);
    {
        s32 v = Script_GetKeywordValue(0x69, 0);
        b3 = q + 0xB3;
        *b3 = v;
    }
    {
        register s32 (*callback)(void) asm("r1") = sub_081F0678;
        asm("" : : "r"(callback));
        *((void **)(q + 0x104)) = callback;
    }
    SceneActors_SetHealthLimits(q, health, health, extra);
    *((u16 *)(p + 0x5F0)) = Div(health, 7);
    *((u16 *)(p + 0x5F2)) = Div(health, 10);
    SingletonSlot7_InitSecondaryCollider(p, extra);
    SingletonSlot7_InitCollider(p);
    SingletonSlot7_InitContactCollider(p, extra);
    SingletonSlot7_RegisterRenderCallback(p);
    SingletonSlot7_InitBackgroundAndMotion(p);
    *((u32 *)(p + 0x38C)) = 0xEBF6;
    *((void **)(p + 0x390)) = ScriptProc_Start(0x922E, 0xEBF6);
    *((u32 *)(p + 0x388)) = 0x2E90;
    SingletonSlot7_InitDisplaySprites(p);
    {
        u32 *flags;
        u8 *s0 = q + 0xAC;
        u8 *s1 = q + 0xAD;
        u8 *s2 = q + 0xAE;
        u32 *s3 = (u32 *)(q + 0xC4);
        u8 *s4 = q + 0xB0;
        s32 i = 15;
        u16 *w = (u16 *)(p + 0x3C6);
        modeField = b3;
        b3 -= 0x17;
        flags = (u32 *)b3;
        do {
            *(w--) = 0x7FFF;
        } while ((--i) >= 0);
        SceneActors_InitResources(p);
        *flags |= 0x10;
        if ((*modeField) == 0) {
            u32 nz = 0;
            register u32 phase asm("r0") = 5;
            asm("" : : "r"(phase));
            *s0 = nz;
            *s1 = nz;
            *s2 = phase;
            *s3 = nz;
            *s4 = 1;
        } else {
            if (hasShared) {
                register u8 byteZero asm("r3") = 0;
                register u32 wordZero asm("r4") = 0;
                asm("" : "+r"(byteZero), "+r"(wordZero));
                *((u16 *)(p + 0x278)) = shared;
                *((s32 *)(p + 0x294)) = *((s16 *)(p + 0x278));
                *((u16 *)(p + 0x5D2)) = 1;
                *s0 = 1;
                *s1 = byteZero;
                *s2 = 3;
                *s3 = wordZero;
                *s4 = 1;
            } else {
                *((u16 *)(p + 0x278)) = hasShared;
                *s0 = 1;
                {
                    typeof(*s1) *q1 = &(*s1);
                    u8 z1 = 2;
                    *q1 = 0;
                    *s2 = z1;
                    *s3 = hasShared;
                    *s4 = 1;
                }
            }
            if (gUnk_020004B8)
                Sound_PlaySong(0x586);
        }
    }
    *((u16 *)(p + 0x5D0)) = 0;
    *((void **)(p + 0x62C)) = SingletonSlot7_CreateEffectPool();
    *((u32 *)(p + 0x5AC)) = 7;
    *((u32 *)(p + 0x5A8)) = 0x80;
    if ((*modeField) && (mode > 0)) {
        CpuSet(p + 0x488, gUnk_03005040, 0x40);
        AnimData_SelectPaletteBank(p + 0x350, 0x1F6);
        CpuSet(*((void **)(p + 0x35C)), p + 0x3C8, 0x10);
        AnimData_SelectPaletteBank(p + 0x36C, 0x1F5);
        CpuSet(*((void **)(p + 0x378)), p + 0x3E8, 0x10);
        *((void **)(p + 0x35C)) = p + 0x3C8;
        *((void **)(p + 0x378)) = p + 0x3E8;
        *((u32 *)(p + 0x5FC)) = 0x1F6;
        *((u32 *)(p + 0x608)) = 0x1F5;
        *((u32 *)(p + 0x304)) = 1;
        *((u32 *)(p + 0x34C)) = 1;
        if (mode == 2) {
            *((u32 *)(p + 0x5A0)) = 0x3C;
            *((u16 *)(p + 0x5E4)) = 1;
        } else {
            *((u16 *)(p + 0x5E4)) = 0;
        }
    } else {
        u8 *src = gUnk_03004FC0 + 0x80;
        u8 *dst = p + 0x508;
        s32 i = 3;
        do {
            CpuSet(src, dst, 0x10);
            dst += 0x20;
            src += 0x20;
        } while ((--i) >= 0);
        *((u32 *)(p + 0x5FC)) = 3;
        *((u32 *)(p + 0x608)) = 2;
    }
    *((u16 *)(p + 0x5E2)) = 0;
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", SingletonSlot7_InitFromScript);
#endif
