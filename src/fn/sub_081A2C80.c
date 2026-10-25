#include "global.h"

// confidence=low: Hresvelgr setup: reads script keywords into the 0x358-0x372 parameter block, creates the pools, sets callbacks and animation state.

// confidence=low: Hresvelgr setup: reads script keywords into the 0x358-0x372 parameter block, creates the pools, sets callbacks and animation state.

// confidence=low: Hresvelgr setup: reads script keywords into the 0x358-0x372 parameter block, creates the pools, sets callbacks and animation state.

// confidence=low: Hresvelgr setup: reads script keywords into the 0x358-0x372 parameter block, creates the pools, sets callbacks and animation state.

// confidence=low: Hresvelgr setup: reads script keywords into the 0x358-0x372 parameter block, creates the pools, sets callbacks and animation state.

// confidence=low: Hresvelgr setup: reads script keywords into the 0x358-0x372 parameter block, creates the pools, sets callbacks and animation state.

// confidence=low: Hresvelgr setup: reads script keywords into the 0x358-0x372 parameter block, creates the pools, sets callbacks and animation state.

// confidence=low: Hresvelgr setup: reads script keywords into the 0x358-0x372 parameter block, creates the pools, sets callbacks and animation state.

// confidence=low: Hresvelgr setup: reads script keywords into the 0x358-0x372 parameter block, creates the pools, sets callbacks and animation state.

// confidence=low: Hresvelgr setup: reads script keywords into the 0x358-0x372 parameter block, creates the pools, sets callbacks and animation state.
extern u32 gUnknown_02000580[4];
struct F2C80Block {
    u32 w[8];
};
extern void *gUnk_02000710;
extern void *gUnk_030042E4;
s32 Script_GetKeywordValue(s32, s32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void SceneActors_Register(void *, s32);
s32 SceneActors_Initialize(void *, void *, s32, void *, s32);
void Hresvelgr_CreateShotPool(s32, s32, s32);
void EffectPool_Create(s32, s32, s32);
void Hresvelgr_CreateProjectilePool(s32, s32, s32);
void Hresvelgr_CreateEffectPool(void);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void SceneActors_SetHealthLimits(void *, s32, s32, s32);
void Hresvelgr_InitializeCollider(void *, u32);
void ColliderActor_InitBodyCollider(void *, s32);
void ColliderActor_InitLowerBodyCollider(void *, s32);
void SceneActors_SetEventScripts(void *, s32, s32);
void SceneActors_SetDamageCallback(void *, void *, void *, s32);
void Hresvelgr_InitializeRadialOffsets(void *);
void ActorAnimation_ResetSequence(void *);
void ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void ScaledShadow_SetGraphic(void *, s32, s32);
void Hresvelgr_ApplyContactDamage(void);
s32 Hresvelgr_ApplyRequestedAction(void *);
void Hresvelgr_BeginDamageReaction(void);
void Hresvelgr_BeginRestAnimation(void);
void Hresvelgr_OnActionTwentyEight(void);
#ifdef NONMATCHING
// fuzzy: 65.6%
// scope shape 182 (differences once low registers are set aside)
// confidence=low: Hresvelgr setup: reads script keywords into the 0x358-0x372 parameter block, creates the pools, sets callbacks and animation state.
s32 sub_081A2C80(void *arg0, void *arg1)
{
    u8 *self = (u8 *)arg0;
    u32 pk[2];
    s32 mode;
    s32 count;
    s32 i;
    s32 sbVal;
    s32 slVal;
    s32 v;
    u8 r5;
    u8 *hdr;
    u32 *w;
    u32 src;
    u32 off;

    mode = Script_GetKeywordValue(0x74, 1);
    if (mode != 1)
        return -1;
    SceneActors_Register(self, 1);
    count = Script_GetKeywordValue(0x50, 1);
    *(u16 *)(self + 0x37C) = count;
    w = (u32 *)(self + 0x10C);
    for (i = 0; i < *(u16 *)(self + 0x37C); i++) {
        src = gUnknown_02000580[i];
        if (src) {
            w[0] = src;
            w[1] = src + 0x30;
        } else {
            w[0] = 0;
            w[1] = 0;
        }
        w++;
    }
    hdr = ScriptProc_Start(0xCB05, 0xF771);
    if (!hdr)
        return -1;
    *(struct F2C80Block *)(self + 0x174) = *(struct F2C80Block *)hdr;
    ResourceHeader_Relocate(self + 0x174, hdr);

    if (Script_SeekToKeyword(0x70)) {
        v = (u16)Script_GetValue();
        pk[0] = (pk[0] & 0xFFFF0000) | v;
        v = Script_GetValue();
        pk[0] = (pk[0] & 0xFFFF) | (v << 16);
        v = (u16)Script_GetValue();
        pk[1] = (pk[1] & 0xFFFF0000) | v;
        v = Script_GetValue();
        r5 = (u8)(((u32)(v << 29) + 0xA0000000u) >> 24);
    } else {
        u16 *s = (u16 *)gUnk_02000710;
        pk[0] = (pk[0] & 0xFFFF0000) | s[0x30 / 2];
        pk[0] = (pk[0] & 0xFFFF) | (s[0x32 / 2] << 16);
        pk[1] = (pk[1] & 0xFFFF0000) | s[0x34 / 2];
        r5 = 0x40;
    }

    if (Script_SeekToKeyword(0x73)) {
        sbVal = Script_GetValue();
        *(u16 *)(self + 0x376) = Script_GetValue();
        *(u16 *)(self + 0x378) = Script_GetValue();
        *(u16 *)(self + 0x374) = Script_GetValue();
        slVal = Script_GetValue();
        *(u16 *)(self + 0x37A) = Script_GetValue();
    } else {
        sbVal = 1;
        *(u16 *)(self + 0x376) = 0;
        *(u16 *)(self + 0x378) = 0;
        *(u16 *)(self + 0x374) = 100;
        slVal = 0;
        *(u16 *)(self + 0x37A) = 0;
    }

    if (Script_SeekToKeyword(0x61)) {
        *(u16 *)(self + 0x358) = Script_GetValue();
        *(u16 *)(self + 0x35A) = Script_GetValue();
        *(u16 *)(self + 0x35C) = Script_GetValue();
        *(u16 *)(self + 0x35E) = Script_GetValue();
        *(u16 *)(self + 0x360) = Script_GetValue();
        *(u16 *)(self + 0x362) = Script_GetValue();
        *(u16 *)(self + 0x364) = Script_GetValue();
        *(u16 *)(self + 0x366) = Script_GetValue();
        *(u16 *)(self + 0x368) = Script_GetValue();
        *(u16 *)(self + 0x36A) = Script_GetValue();
        *(u16 *)(self + 0x36C) = Script_GetValue();
        *(u16 *)(self + 0x36E) = Script_GetValue();
        *(u16 *)(self + 0x370) = Script_GetValue();
        *(u16 *)(self + 0x372) = Script_GetValue();
    } else {
        *(u16 *)(self + 0x358) = 0x32;
        *(u16 *)(self + 0x35A) = 0x50;
        *(u16 *)(self + 0x35C) = 0x3C;
        *(u16 *)(self + 0x35E) = 8;
        *(u16 *)(self + 0x360) = 0x28;
        *(u16 *)(self + 0x362) = 0xF;
        *(u16 *)(self + 0x364) = 0x50;
        *(u16 *)(self + 0x366) = 0x50;
        *(u16 *)(self + 0x368) = 0x3C;
        *(u16 *)(self + 0x36A) = 0x80;
        *(u16 *)(self + 0x36C) = 0x50;
        *(u16 *)(self + 0x36E) = 0x3C;
        *(u16 *)(self + 0x370) = 0x78;
        *(u16 *)(self + 0x372) = 8;
    }

    Hresvelgr_CreateShotPool(*(s16 *)(self + 0x35E), *(s16 *)(self + 0x360),
                             *(s16 *)(self + 0x362));
    EffectPool_Create(*(s16 *)(self + 0x364), *(s16 *)(self + 0x366), *(s16 *)(self + 0x368));
    Hresvelgr_CreateProjectilePool(*(s16 *)(self + 0x36A), *(s16 *)(self + 0x36C),
                                   *(s16 *)(self + 0x36E));
    Hresvelgr_CreateEffectPool();
    SceneActors_Initialize(self, arg1, mode, pk, r5);
    *(u8 *)(self + 0x32B) = (u8)(*(u8 *)(self + 0xA8) >> 1);
    Widget_InitLinked(self + 0x194, self + 0x174, 0, 0, 2, 0, 0x3C, pk);
    SceneActors_StartUnmirroredAnimation(self, self + 0x194, self + 0x174, 0x18, 1, 0);
    *(void **)(self + 0x104) = (void *)0x081A1EFD;
    *(void **)(self + 0x108) = (void *)Hresvelgr_ApplyRequestedAction;
    SceneActors_SetHealthLimits(self, sbVal, sbVal, *(s16 *)(self + 0x376));
    Hresvelgr_InitializeCollider(self, (u16)(u32)arg1);
    ColliderActor_InitBodyCollider(self, *(s16 *)(self + 0x378));
    ColliderActor_InitLowerBodyCollider(self, *(s16 *)(self + 0x376));
    *(u32 *)(self + 0x7E4) = Script_GetKeywordValue(0x45, 0);
    if (Script_SeekToKeyword(0x63)) {
        s32 ev1 = Script_GetValue();
        s32 ev2 = Script_GetValue();
        SceneActors_SetEventScripts(self, ev1, ev2);
    } else {
        SceneActors_SetEventScripts(self, 0, 0);
    }
    if (Script_SeekToKeyword(0x41)) {
        u32 *ev = (u32 *)(self + 0xF0);
        for (i = 3; i >= 0; i--)
            *ev++ = Script_GetValue();
    }
    off = 0;
    for (i = 15; i >= 0; i--) {
        CpuSet((u8 *)gUnk_030042E4 + 0x5E00 + off, self + 0x424 + off, 0x04000008);
        off += 0x20;
    }
    {
        for (i = 0; i <= 0x6F; i++)
            *(u16 *)(self + 0x704 + ((i & 0xF) << 1) + ((i >> 4) << 5)) = 0x1084;
    }
    *(u16 *)(self + 0x1CE) = 0x2F0;
    *(void **)(self + 0x1DC) = self + 0x424;
    ScaledShadow_Initialize(self + 0x3B4, self + 0x1B4, 0x40, 0x40, 0x40, 0x40, 0x2000, 1, 6, 0, 0);
    ScaledShadow_SetGraphic(self + 0x3B4, 0x1C1B, 7);
    *(u16 *)(self + 0x37E) = 0;
    *(u16 *)(self + 0x380) = 0;
    *(u8 *)(self + 0x382) = 0;
    SceneActors_SetDamageCallback(self, self + 0x248, (void *)Hresvelgr_BeginDamageReaction, slVal);
    Hresvelgr_InitializeRadialOffsets(self);
    ActorAnimation_ResetSequence(self);
    *(u32 *)(self + 0x420) = 0;

    if (Script_SeekToKeyword(0x44)) {
        s32 t = Script_GetValue();
        if (t == 0) {
            *(u8 *)(self + 0xAC) = 2;
            *(u8 *)(self + 0xAD) = 0;
            *(u8 *)(self + 0xAE) = 1;
            *(u32 *)(self + 0xC4) = 0;
            *(u8 *)(self + 0xB0) = 1;
            *(u8 *)(self + 0xAB) = 0x1D;
            *(void **)(self + 0x100) = (void *)Hresvelgr_BeginRestAnimation;
            *(u32 *)(self + 0x90) = 0;
            *(u8 *)(self + 0xB1) = 1;
            *(u8 *)(self + 0xA2) = 0;
            *(u32 *)(self + 0xA4) = 0;
        } else if (t == 1) {
            *(u8 *)(self + 0xAC) = 2;
            *(u8 *)(self + 0xAD) = 0;
            *(u8 *)(self + 0xAE) = 0;
            *(u32 *)(self + 0xC4) = 0;
            *(u8 *)(self + 0xB0) = 1;
            *(u8 *)(self + 0xAB) = 0x1C;
            *(void **)(self + 0x100) = (void *)Hresvelgr_OnActionTwentyEight;
            *(u32 *)(self + 0x90) = 0;
            *(u8 *)(self + 0xB1) = 1;
            *(u8 *)(self + 0xA2) = 0;
            *(u32 *)(self + 0xA4) = 0;
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081A2C80);
#endif
