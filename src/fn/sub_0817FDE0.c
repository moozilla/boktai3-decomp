#include "global.h"

#define FDE0_AT(T, off) (*(T *)(arg0 + (off)))

struct FDE0Header {
    u32 w[8];
};

s32 Script_SeekToKeyword(u32);
s32 Script_GetKeywordValue(u32, s32);
s32 Script_GetValue(void);
u32 Script_GetPc(void);
void SceneActors_Register(u8 *, s32);
void ResourceHeader_Relocate(void *, void *);
void *ScriptProc_Start(u32, u32);
void SceneActors_Initialize(u8 *, u32, s32, void *, u32);
void CellObject_Init(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, u32, u32);
void TerrainOverlay_QueueAddActor(void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void Anim_SelectPaletteBank(void *, s32);
void SceneActors_StartUnmirroredAnimation(u8 *, void *, void *, s32, s32, s32);
void SpriteComponent_Init(void *);
void SpriteComponent_ConfigureCollider(void *, s32, s32, s32);
void *FrostEmitter_Create(void);
void ActorAnimation_UpdateDirectional(void);
void BossSlot2_ApplyPendingDamage(void);
void BossSlot2_InitStateDispatch(void);
void SceneActors_SetHealthLimits(u8 *, s32, s32, s32);
void ActorImpact_InitAttackCollider(u8 *);
void ActorImpact_InitBodyCollider(u8 *, s32);
void ActorImpact_InitCollider(u8 *);
void SceneActors_SetEventScripts(u8 *, s32, s32);
void SceneActors_InitConvergingParticles(u8 *);
void Shadow_Initialize(void *, void *, s32);
void ProximityVolume_InitializeCallbacks(void *);
void ProximityVolume_SetParameter(void *, s32);
void CameraFollow_Initialize(void *, u32, void *, s32);
void SceneActors_SetDamageCallback(u8 *, void *, void *, s32);
void sub_0817A7C8(void);
void ProximityVolume_TranslateBounds(void *, void *);
void Task4E01_Destroy(void);
void *TimedEffect_GetOrCreateManager(s32);

#ifdef NONMATCHING
// fuzzy: 56.6%
// scope shape 279 (differences once low registers are set aside)
// confidence=low: boss actor setup from its script, sprite and collider init, VRAM copies
s32 sub_0817FDE0(u8 *arg0, u32 arg1)
{
    u32 pos[2];
    s32 mode;
    s32 count;
    s32 i;
    s32 sl;
    u32 flags;
    u8 *res;
    u32 *dst;
    u32 *src;
    u8 *gp;

    gp = *(u8 **)0x02000710;
    *(u32 *)(gp + 0x868) |= 0x400;

    mode = Script_GetKeywordValue(0x74, 2);
    if (mode != 2)
        return -1;
    SceneActors_Register(arg0, 2);
    FDE0_AT(u16, 0x4F6) = Script_GetKeywordValue(0x42, 0);
    FDE0_AT(u8, 0xB3) = Script_GetKeywordValue(0x69, 0);
    count = Script_GetKeywordValue(0x50, 1);
    FDE0_AT(u8, 0x174) = count;
    dst = (u32 *)(arg0 + 0x10C);
    src = (u32 *)0x02000580;
    for (i = 0; i < FDE0_AT(u8, 0x174); i++) {
        if (*src != 0) {
            dst[0] = *src;
            dst[1] = *src + 0x30;
        } else {
            dst[0] = 0;
            dst[1] = 0;
        }
        dst++;
        src++;
    }

    res = ScriptProc_Start(0xCB05, 0xDE23);
    if (res == 0)
        return -1;
    *(struct FDE0Header *)(arg0 + 0x2AC) = *(struct FDE0Header *)res;
    ResourceHeader_Relocate(arg0 + 0x2AC, res);

    if (Script_SeekToKeyword(0x70)) {
        pos[0] = (pos[0] & 0xFFFF0000) | (u16)Script_GetValue();
        pos[0] = (pos[0] & 0x0000FFFF) | (Script_GetValue() << 16);
        pos[1] = (pos[1] & 0xFFFF0000) | (u16)Script_GetValue();
        flags = ((Script_GetValue() << 29) + 0xA0000000u) >> 24;
    } else {
        pos[0] = (pos[0] & 0xFFFF0000) | *(u16 *)(gp + 0x30);
        pos[0] = (pos[0] & 0x0000FFFF) | (*(u16 *)(gp + 0x32) << 16);
        pos[1] = (pos[1] & 0xFFFF0000) | *(u16 *)(gp + 0x34);
        flags = 0x40;
    }
    FDE0_AT(s16, 0x4F2) = (s16)(pos[0] >> 16);

    if (Script_SeekToKeyword(0x73)) {
        sl = Script_GetValue();
        FDE0_AT(u16, 0x274) = Script_GetValue();
        FDE0_AT(u16, 0x276) = Script_GetValue();
        FDE0_AT(u16, 0x176) = Script_GetValue();
    } else {
        sl = 0x3E8;
        FDE0_AT(u16, 0x176) = 0x14;
    }

    if (Script_SeekToKeyword(0x61)) {
        FDE0_AT(u16, 0x4F8) = Script_GetValue();
        FDE0_AT(u16, 0x4FA) = Script_GetValue();
        FDE0_AT(u16, 0x4FC) = Script_GetValue();
        FDE0_AT(u16, 0x4FE) = Script_GetValue();
        FDE0_AT(u16, 0x500) = Script_GetValue();
    } else {
        FDE0_AT(u16, 0x4F8) = 0x10;
        FDE0_AT(u16, 0x4FA) = 0x10;
        FDE0_AT(u16, 0x4FC) = 0x10;
        FDE0_AT(u16, 0x4FE) = 0x10;
        FDE0_AT(u16, 0x500) = 0x20;
    }

    SceneActors_Initialize(arg0, arg1, mode, pos, flags);
    CellObject_Init(arg0 + 0x278, pos);
    SceneBody_SetMovementProbe(arg0 + 0x48, arg0 + 0x278, 0x23, 0x23);
    TerrainOverlay_QueueAddActor(arg0 + 0x48);
    Widget_InitLinked(arg0 + 0x2CC, arg0 + 0x2AC, 0, 0, 2, 0, 0x3C, pos);
    Anim_SelectPaletteBank(arg0 + 0x2CC, 0x34);
    SceneActors_StartUnmirroredAnimation(arg0, arg0 + 0x2CC, arg0 + 0x2AC, 0xF4, 1, 0);
    SpriteComponent_Init(arg0 + 0x32C);
    SpriteComponent_ConfigureCollider(arg0 + 0x32C, *(s16 *)(arg0 + 0x4FA), 0x40, 0x1E);
    FDE0_AT(void *, 0x41C) = FrostEmitter_Create();

    FDE0_AT(u8, 0x4EA) = 0;
    FDE0_AT(u8, 0xAC) = 0;
    FDE0_AT(u8, 0xAD) = 0;
    FDE0_AT(u8, 0xAE) = 0;
    FDE0_AT(u32, 0xC4) = 0;
    FDE0_AT(u8, 0xB0) = 1;
    FDE0_AT(u8, 0xAB) = 1;
    FDE0_AT(void *, 0x100) = ActorAnimation_UpdateDirectional;
    FDE0_AT(u32, 0x90) = 0;
    FDE0_AT(u8, 0xB1) = 1;
    FDE0_AT(u8, 0xA2) = 0;
    FDE0_AT(u32, 0xA4) = 0;
    FDE0_AT(void *, 0x104) = BossSlot2_ApplyPendingDamage;
    FDE0_AT(void *, 0x108) = BossSlot2_InitStateDispatch;

    SceneActors_SetHealthLimits(arg0, sl, sl, 0);
    ActorImpact_InitAttackCollider(arg0);
    ActorImpact_InitBodyCollider(arg0, 0);
    ActorImpact_InitCollider(arg0);

    if (Script_SeekToKeyword(0x65)) {
        u32 *p = (u32 *)(arg0 + 0xC60);
        p[0] = Script_GetValue();
        p[1] = Script_GetValue();
    } else {
        *(u32 *)(arg0 + 0xC64) = 0;
        *(u32 *)(arg0 + 0xC60) = 0;
    }

    if (Script_SeekToKeyword(0x63)) {
        s32 a = Script_GetValue();
        s32 b = Script_GetValue();
        SceneActors_SetEventScripts(arg0, a, b);
    } else {
        SceneActors_SetEventScripts(arg0, 0, 0);
    }

    if (Script_SeekToKeyword(0x41)) {
        u32 *p = (u32 *)(arg0 + 0xF0);
        for (i = 0; i <= 3 && Script_GetPc() != 0; i++)
            *p++ = Script_GetValue();
    }

    SceneActors_InitConvergingParticles(arg0);
    Shadow_Initialize(arg0 + 0x3DC, arg0 + 0x2EC, 0);
    ProximityVolume_InitializeCallbacks(arg0 + 0x428);
    ProximityVolume_SetParameter(arg0 + 0x428, *(s16 *)(arg0 + 0x4FE));
    FDE0_AT(u8, 0x4E0) = 1;
    FDE0_AT(u8, 0x4E1) = 0;
    FDE0_AT(u8, 0x4E5) = 0;
    FDE0_AT(u16, 0x4E6) = 0;

    {
        u8 *srcp;
        u8 *dstp;
        u32 off = 0x680;
        dstp = arg0 + 0x600;
        for (i = 15; i >= 0; i--) {
            CpuSet(*(u8 **)0x030042E4 + off, dstp, 0x04000008);
            dstp += 0x20;
            off += 0x20;
        }
        srcp = *(u8 **)0x030042E4 + 0x3240;
        dstp = arg0 + 0x800;
        for (i = 15; i >= 0; i--) {
            CpuSet(srcp, dstp, 0x04000008);
            dstp += 0x20;
        }
        srcp = *(u8 **)0x030042E4 + 0x3240;
        dstp = arg0 + 0xA00;
        for (i = 15; i >= 0; i--) {
            CpuSet(srcp, dstp, 0x04000008);
            dstp += 0x20;
        }
        CpuSet(srcp, arg0 + 0xC00, 0x04000008);
    }

    *(u8 **)(arg0 + 0x314) = arg0 + 0x600;
    FDE0_AT(u8, 0x506) = 0;
    FDE0_AT(u32, 0x508) = 0x34;
    FDE0_AT(u8, 0x504) = 0;
    FDE0_AT(u8, 0x507) = 5;
    FDE0_AT(u8, 0x50C) = 1;
    CameraFollow_Initialize(arg0 + 0xC30, FDE0_AT(u32, 0x110), arg0 + 0x2EC, 1);
    *(u32 *)(arg0 + 0xC40) = 0;
    *(u32 *)(arg0 + 0xC44) = 0x8C;
    SceneActors_SetDamageCallback(arg0, arg0 + 0x1CC, sub_0817A7C8, FDE0_AT(u16, 0x176));
    ProximityVolume_TranslateBounds(arg0 + 0x288, arg0 + 0x54);
    Task4E01_Destroy();
    TimedEffect_GetOrCreateManager(0x11);
    FDE0_AT(u8, 0x476) = 0;
    FDE0_AT(u32, 0x464) = 0;
    FDE0_AT(u8, 0x51F) = 0;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0817FDE0);
#endif
