#include "global.h"
#include "scene_actor_reaction.h"
#include "engine.h"
#include "scene_vec.h"
#include "render.h"

void SceneActors_Register(void *, s32);
void SceneActors_Initialize(void *, void *, s32, void *, s32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void SceneActors_SetHealthLimits(void *, s32, s32, s32);
void SceneActors_SetDamageCallback(void *, void *, void *, s32);
void SceneResource_InitializeHiddenSprite(void *);
void Math_SetMotionPosition(void *, void *);
void *Path_GetDefinition(s32);
void *Path_GetPointData(void *);
void Path_GetPointPosition(void *, void *, s32);
void Ratatosk_InitHurtCollider(void *, s32);
void Ratatosk_InitBodyCollider(void *);
void Ratatosk_InitFrontCollider(void *);
void Ratatosk_InitWideBodyCollider(void *, s32);
void Ratatosk_InitOuterCollider(void *);
void CameraFollow_Initialize(void *, s32, void *, s32);
void ScaledShadow_InitializeDefault(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
void *HomingProjectile_CreateManager(void);
void *PlayerClone_CreatePool(void);
void *SceneProjectile_CreatePool(void);
void *SunOrb_GetOrCreateManager(void);
void *Ratatosk_CreateAnimatedParticlePool(void);
void *Ratatosk_CreateParticlePool(void);
s32 Mod(s32, s32);
extern void *gUnknown_02000580;

#define H8(off)  (*(u8 *)(a + (off)))
#define H16(off) (*(u16 *)(a + (off)))
#define H32(off) (*(u32 *)(a + (off)))

#ifdef NONMATCHING
// fuzzy: 53.0%
// scope shape 281 (differences once low registers are set aside)
// confidence=low: Ratatosk boss setup. Reads the scene script keywords into the actor (ranges, timings, path points,
// palette copies, collider and projectile pools) and returns -1 when a pool could not be created.
s32 sub_081FAE0C(void *arg0, void *arg1)
{
    u8 *a = arg0;
    struct ContactPosition sizes;
    struct ContactPosition motion;
    u32 zero;
    u32 hpA, hpB;
    s32 r5;
    s32 i;
    s32 found;
    u8 *p44;
    u8 *base;
    u32 pv[2];
    u32 *zc;
    s32 k;
    s32 v44;
    void *g;
    void *pdata;

    r5 = 0;
    hpA = 0x7D0;
    zero = 0;
    SceneActors_Register(a, 4);
    CpuSet(&zero, &sizes, 0x05000002);
    sizes.x = 0x800;
    sizes.y = 0x200;
    sizes.z = 0x400;
    hpB = zero;

    if (Script_SeekToKeyword(0x65)) {
        H32(0xE8) = Script_GetValue();
        H32(0xEC) = Script_GetValue();
    } else {
        H32(0xE8) = zero;
        H32(0xEC) = zero;
    }
    if (Script_SeekToKeyword(0x6C)) {
        hpA = Script_GetValue();
        hpB = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x70)) {
        sizes.x = Script_GetValue();
        sizes.y = Script_GetValue();
        sizes.z = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x72)) {
        motion.x = Script_GetValue();
        motion.y = Script_GetValue();
        motion.z = Script_GetValue();
    } else {
        motion.x = 0x800;
        motion.y = 0x80;
        motion.z = 0x800;
    }
    Math_SetMotionPosition(a + 0x3AC, &motion);

    if (Script_SeekToKeyword(0x6D)) {
        for (i = 0; i < 6; i++) {
            H16(0xCCC + i * 8) = Script_GetValue();
            H16(0xCCE + i * 8) = Script_GetValue();
            H16(0xCD0 + i * 8) = Script_GetValue();
        }
    }
    if (Script_SeekToKeyword(0x41)) {
        H16(0xE9C) = Script_GetValue();
        H16(0xE9E) = Script_GetValue();
        H16(0xEA0) = Script_GetValue();
        H16(0xEA2) = Script_GetValue();
        H16(0xEA4) = Script_GetValue();
        H16(0xEA6) = Script_GetValue();
    } else {
        H16(0xE9C) = 0x1E;
        H16(0xE9E) = 0xF;
        H16(0xEA0) = 0x1E;
        H16(0xEA2) = 0x28;
        H16(0xEA4) = 0x1E;
        H16(0xEA6) = 0x1E;
    }
    if (Script_SeekToKeyword(0x63)) {
        H16(0xEAE) = Script_GetValue();
        H16(0xEB0) = Script_GetValue();
        H16(0xEB2) = Script_GetValue();
    } else {
        H16(0xEAE) = 5;
        H16(0xEB0) = 0;
        H16(0xEB2) = 0x14;
    }
    if (Script_SeekToKeyword(0x42)) {
        H16(0xEA8) = Script_GetValue();
        H16(0xEAA) = Script_GetValue();
        H16(0xEAC) = Script_GetValue();
    } else {
        H16(0xEA8) = 0x1E;
        H16(0xEAA) = 0x1E;
        H16(0xEAC) = 0x1E;
    }
    if (Script_SeekToKeyword(0x53)) {
        H16(0xEB4) = Script_GetValue();
        H16(0xEB6) = Script_GetValue();
        H16(0xEB8) = Script_GetValue();
        H16(0xEBA) = Script_GetValue();
        H16(0xEBC) = Script_GetValue();
        H16(0xEBE) = Script_GetValue();
        H16(0xEC0) = Script_GetValue();
        H16(0xEC2) = Script_GetValue();
        H16(0xEC4) = Script_GetValue();
    } else {
        H16(0xEB4) = 0x1E;
        H16(0xEB6) = 0xA;
        H16(0xEB8) = 0x1E;
        H16(0xEBA) = 0x32;
        H16(0xEBC) = 0x1E;
        H16(0xEBE) = 0x1E;
        H16(0xEC0) = 0x1E;
        H16(0xEC2) = 0x1E;
        H16(0xEC4) = 0x1E;
    }
    if (Script_SeekToKeyword(0x73)) {
        H16(0xEC6) = Script_GetValue();
        H16(0xEC8) = Script_GetValue();
        H16(0xECA) = Script_GetValue();
        H16(0xECC) = Script_GetValue();
        H16(0xECE) = Script_GetValue();
        H16(0xED0) = Script_GetValue();
    } else {
        H16(0xEC6) = 0x1E;
        H16(0xEC8) = 0x1E;
        H16(0xECA) = 0x1E;
        H16(0xECC) = 0x1E;
        H16(0xECE) = 0x1E;
        H16(0xED0) = 0x1E;
    }
    if (Script_SeekToKeyword(0x52)) {
        H16(0xEFC) = Script_GetValue();
        H16(0xEFE) = Script_GetValue();
        H16(0xF00) = Script_GetValue();
    } else {
        H16(0xEFC) = 0x1E;
        H16(0xEFE) = 5;
        H16(0xF00) = 0x1E;
    }
    if (Script_SeekToKeyword(0x47)) {
        H16(0xEE6) = Script_GetValue();
    } else {
        H16(0xEE6) = 0x14;
    }
    if (Script_SeekToKeyword(0x62)) {
        for (i = 0; i < 6; i++) {
            H16(0xD1C + i * 0x18) = Script_GetValue();
            H16(0xD1E + i * 0x18) = Script_GetValue();
            H16(0xD20 + i * 0x18) = Script_GetValue();
        }
    }
    if (Script_SeekToKeyword(0x44)) {
        v44 = Script_GetValue();
    } else {
        v44 = 0;
    }
    if (Script_SeekToKeyword(0x64)) {
        r5 = (u8)(((Script_GetValue() << 29) + 0xA0000000u) >> 24);
    }
    if (Script_SeekToKeyword(0x58)) {
        H16(0xED2) = Script_GetValue();
    } else {
        H16(0xED2) = 0x3E8;
    }
    found = Script_SeekToKeyword(0x74);
    if (found) {
        H16(0xF0C) = Script_GetValue();
    } else {
        H16(0xF0C) = found;
    }
    g = gUnknown_02000580;
    if (g) {
        H32(0x10C) = (u32)g;
        H32(0x110) = (u32)g + 0x30;
    } else {
        H32(0x10C) = 0;
        H32(0x110) = 0;
    }
    SceneActors_Initialize(a, arg1, 4, &sizes, r5);
    H8(0x114) = r5;
    H32(0x494) = 0xFB81;
    SceneResource_InitializeHiddenSprite(a);
    SceneActors_StartUnmirroredAnimation(a, a + 0x34C, a + 0x32C, 7, 1, 0);

    p44 = a + 0xB3;
    *p44 = Script_GetKeywordValue(0x69, 0);
    if (*p44 == 0) {
        pdata = Path_GetPointData(Path_GetDefinition(0));
        for (i = 0; i <= 8; i++) {
            Path_GetPointPosition((void *)pv, pdata, i);
            H32(0xCCC + i * 8) = pv[0];
            H32(0xCCC + i * 8 + 4) = pv[1];
            H16(0xCCC + i * 8 + 2) = 0x200;
        }
        pdata = Path_GetPointData(Path_GetDefinition(1));
        for (i = 0; i <= 8; i++) {
            Path_GetPointPosition((void *)pv, pdata, i);
            H32(0xD1C + i * 0x18) = pv[0];
            H32(0xD1C + i * 0x18 + 4) = pv[1];
            H16(0xD1C + i * 0x18 + 2) = 0x200;
        }
    }
    H32(0x104) = 0x81F6429;
    H32(0x108) = 0x81F63D1;
    SceneActors_SetHealthLimits(a, hpA, hpA, hpB);

    if (!Script_SeekToKeyword(0x70)) {
        H32(0x54) = H32(0xCCC);
        H32(0x58) = H32(0xCD0);
    }
    Ratatosk_InitHurtCollider(a, zero);
    Ratatosk_InitBodyCollider(a);
    Ratatosk_InitFrontCollider(a);
    Ratatosk_InitWideBodyCollider(a, zero);
    Ratatosk_InitOuterCollider(a);

    base = gPaletteBankData;
    for (i = 0; i < 16; i++) {
        CpuSet(base + 0x3E40 + i * 0x20, a + 0x4B8 + i * 0x20, 0x4000008);
    }
    base = gPaletteBankData;
    for (i = 0; i < 16; i++) {
        CpuSet(base + 0x3E40 + i * 0x20, a + 0x6B8 + i * 0x20, 0x4000008);
    }
    H16(0x6C2) = 0x1F;
    base = gPaletteBankData;
    for (i = 0; i < 16; i++) {
        CpuSet(base + 0x3240, a + 0x8B8 + i * 0x20, 0x4000008);
    }
    H32(0x394) = (u32)(a + 0x4B8);
    H8(0x4AE) = 0;
    H32(0x4B0) = 0x1F2;
    H8(0x4AC) = 0;
    H8(0x4AF) = 5;
    H32(0xF04) = 0x3C;
    H32(0x9C) |= 0x10;

    if (*p44 == 0) {
        H8(0xAC) = 0;
        H8(0xAD) = 0;
        H8(0xAE) = 1;
        H32(0xC4) = 0;
        k = 1;
    } else if (v44 == 0) {
        H8(0xAC) = 2;
        H8(0xAD) = 0;
        H8(0xAE) = 1;
        H32(0xC4) = 0;
        k = 1;
    } else {
        H8(0xAC) = 2;
        H8(0xAD) = 0;
        H8(0xAE) = 0;
        H32(0xC4) = 0;
        k = 1;
    }
    H8(0xB0) = k;
    H16(0xED4) = 0;
    H16(0xE9C) = 0x1E;
    H16(0xE9E) = 0xF;
    H16(0xE52) = 0;
    H16(0xE54) = 0;
    H16(0xE58) = 0;
    H16(0xEDA) = 3;
    H16(0xEDC) = 3;
    H16(0xE6C) = 0x40;
    H16(0xE6C) = 0x80;
    H32(0xEF0) = 2;

    zc = &gRandomIndex;
    *zc = (*zc + 1) & 0x3FF;
    k = Mod(gUnk_0203B400[*zc], 9);
    H32(0xE5C) = H32(0xCCC + k * 8);
    H32(0xE60) = H32(0xCCC + k * 8 + 4);

    if (*p44 == 0) {
        CameraFollow_Initialize(a + 0x3DC, *(u32 *)(a + 0x110), a + 0x54, 1);
        *(u32 *)(a + 0x3DC + 0x10) = 0x8C;
        *(u32 *)(a + 0x3DC + 0x14) = 0xFF;
        *(u32 *)(a + 0x3DC + 0x2C) = 0x1E;
    }
    SceneActors_SetDamageCallback(a, a + 0x188, (void *)0x081F9F11, 0);
    ScaledShadow_InitializeDefault(a + 0x40C, a + 0x36C);
    if (*p44 != 0)
        TerrainOverlay_QueueAddActor(a + 0x48);
    if (*p44 == 0) {
        H32(0xF18) = (u32)HomingProjectile_CreateManager();
        if (H32(0xF18) == 0)
            return -1;
        H32(0xF1C) = (u32)PlayerClone_CreatePool();
        if (H32(0xF1C) == 0)
            return -1;
    }
    H32(0xF28) = (u32)SceneProjectile_CreatePool();
    if (H32(0xF28) == 0)
        return -1;
    H32(0xF2C) = (u32)SunOrb_GetOrCreateManager();
    if (H32(0xF2C) == 0)
        return -1;
    H32(0xF24) = (u32)Ratatosk_CreateAnimatedParticlePool();
    if (H32(0xF24) == 0)
        return -1;
    H32(0xF20) = (u32)Ratatosk_CreateParticlePool();
    if (H32(0xF20) == 0)
        return -1;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081FAE0C);
#endif
