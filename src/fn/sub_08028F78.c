#include "global.h"
#include "render.h"

/* Pair entries and resource copy follow the ROM's sliding word stores. */
/* confidence=low: Vampire actor setup from its script block (keywords 0x74..0x72), then graphics, event and contact objects. */

void SceneActorAnimation_UpdateDirect(void);
void SceneActorAnimation_UpdateDir(void);
void SceneActorAnimation_UpdateEightDirect(void);
void SceneActorAnimation_UpdateEig(void);

/* Pair entries and resource copy follow the ROM's sliding word stores. */
/* confidence=low: Vampire actor setup from its script block (keywords 0x74..0x72), then graphics, event and contact objects. */

/* Pair entries and resource copy follow the ROM's sliding word stores. */
/* confidence=low: Vampire actor setup from its script block (keywords 0x74..0x72), then graphics, event and contact objects. */

/* Pair entries and resource copy follow the ROM's sliding word stores. */
/* confidence=low: Vampire actor setup from its script block (keywords 0x74..0x72), then graphics, event and contact objects. */

/* Pair entries and resource copy follow the ROM's sliding word stores. */
/* confidence=low: Vampire actor setup from its script block (keywords 0x74..0x72), then graphics, event and contact objects. */

/* Pair entries and resource copy follow the ROM's sliding word stores. */
/* confidence=low: Vampire actor setup from its script block (keywords 0x74..0x72), then graphics, event and contact objects. */

/* Pair entries and resource copy follow the ROM's sliding word stores. */
/* confidence=low: Vampire actor setup from its script block (keywords 0x74..0x72), then graphics, event and contact objects. */
extern u8 *gUnknown_02000580[];
extern u16 gUnk_02000710[];
extern const void *gUnk_0824DC78;
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
u32 Script_SeekToKeyword(u32);
u32 Script_GetValue(void);
u32 Script_GetKeywordValue(u32, u32);
u32 Script_GetPc(void);
void SceneActors_Register(void *, u32);
void SceneActors_Initialize(void *, u32, u32, void *, u32);
void SceneBody_SetPositionOffset(void *, const void *);
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, s32, const void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, u32);
void SceneActors_SetHealthLimits(void *, u32, u32, u32);
void SceneActorGraphics_InitializeBody(void *);
void SceneActorGraphics_InitializeOverlay(void *, u32);
void SceneActors_SetEventScripts(void *, u32, u32);
void ScaledShadow_Initialize(void *, void *, u32, u32);
void ScaledShadow_SetGraphic(void *, u32, u32);
void Math_SetMotionPosition(void *, void *);
void *BackgroundFlash_Create(s16, s16);
void *ContactSwarm_Create(void);
void *ContactActorPool_Create(u32, u32, u32);
void CameraFollow_Initialize(void *, void *, void *, u32);
void SceneActors_SetDamageCallback(void *, void *, void *, u32);
void CpuSet(const void *, void *, u32);
void Vampire_ApplyPendingDamage(void);
void SceneActors_TryApplyRequestedState(void);
void SceneActorAnimation_UpdateDirectionalCallback(void);
void SceneActorAnimation_UpdateEightDirectionCallback(void);
void SceneActors_UpdateStationaryReaction(void);
void sub_08022B18(void);

struct Sub08028F78Pos {
    s16 x, y, z, pad;
};

/* Pair entries and resource copy follow the ROM's sliding word stores. */
/* confidence=low: Vampire actor setup from its script block (keywords 0x74..0x72), then graphics, event and contact objects. */
#ifdef NONMATCHING
// fuzzy: 44.4%
// scope shape 370 (differences once low registers are set aside)
int sub_08028F78(void *arg0, void *arg1)
{
    u8 *s = (u8 *)arg0;
    u32 zeroKw;
    u32 i, n;
    u32 *resource;
    u32 *dst;
    u8 **src;
    struct Sub08028F78Pos pos[1];
    struct Sub08028F78Pos pos2[1];
    u32 zero;
    u32 mode;
    u32 flags;
    u32 r5;
    u8 *palette;

    zeroKw = Script_GetKeywordValue(0x74, 0);
    if (zeroKw)
        return -1;
    SceneActors_Register(s, 0);
    *(u32 *)(s + 0x358) = Script_GetKeywordValue(0x42, 0);
    s[0xB3] = Script_GetKeywordValue(0x69, 0);
    s[0x174] = Script_GetKeywordValue(0x50, 1);

    n = s[0x174];
    dst = (u32 *)(s + 0x10C);
    src = gUnknown_02000580;
    for (i = 0; i < n; i++) {
        u8 *q = src[i];
        if (q) {
            dst[0] = (u32)q;
            dst[1] = (u32)(q + 0x30);
        } else {
            dst[0] = 0;
            dst[1] = 0;
        }
        dst++;
    }

    resource = (u32 *)ScriptProc_Start(0xCB05, 0xF761);
    if (!resource)
        return -1;
    {
        u32 *d = (u32 *)(s + 0x220);
        u32 *sr = resource;
        d[0] = sr[0];
        d[1] = sr[1];
        d[2] = sr[2];
        d[3] = sr[3];
        d[4] = sr[4];
        d[5] = sr[5];
        d[6] = sr[6];
        d[7] = sr[7];
    }
    ResourceHeader_Relocate(s + 0x220, resource);

    if (Script_SeekToKeyword(0x70)) {
        pos[0].x = Script_GetValue();
        pos[0].y = Script_GetValue();
        pos[0].z = Script_GetValue();
        r5 = (u8)((Script_GetValue() << 29) + 0xA0000000) >> 24;
    } else {
        u16 *cfg = (u16 *)gUnk_02000710;
        pos[0].x = *(u16 *)((u8 *)cfg + 0x30);
        pos[0].y = *(u16 *)((u8 *)cfg + 0x32);
        pos[0].z = *(u16 *)((u8 *)cfg + 0x34);
        r5 = 0x40;
    }
    *(s16 *)(s + 0x352) = pos[0].x;

    if (Script_SeekToKeyword(0x72)) {
        pos2[0].x = Script_GetValue();
        pos2[0].y = Script_GetValue();
        pos2[0].z = Script_GetValue();
    } else {
        pos2[0].x = 0x600;
        pos2[0].y = 0x100;
        pos2[0].z = 0x600;
    }
    pos2[0].y = *(u16 *)(s + 0x352);
    Math_SetMotionPosition(s + 0x30C, pos2);

    if (Script_SeekToKeyword(0x73)) {
        flags = Script_GetValue();
        *(u16 *)(s + 0x176) = Script_GetValue();
        *(u16 *)(s + 0x374) = Script_GetValue();
        *(u16 *)(s + 0x376) = Script_GetValue();
    } else {
        flags = 0x3E8;
        *(u16 *)(s + 0x176) = 0x14;
        *(u16 *)(s + 0x374) = 0x1A4;
        *(u16 *)(s + 0x376) = 5;
    }

    if (Script_SeekToKeyword(0x61)) {
        *(u16 *)(s + 0x35C) = Script_GetValue();
        *(u16 *)(s + 0x35E) = Script_GetValue();
        *(u16 *)(s + 0x360) = Script_GetValue();
        *(u16 *)(s + 0x362) = Script_GetValue();
        *(u16 *)(s + 0x364) = Script_GetValue();
        *(u16 *)(s + 0x366) = Script_GetValue();
        *(u16 *)(s + 0x368) = Script_GetValue();
        *(u16 *)(s + 0x36A) = Script_GetValue();
        *(u16 *)(s + 0x36C) = Script_GetValue();
        *(u16 *)(s + 0x36E) = Script_GetValue();
        *(u16 *)(s + 0x370) = Script_GetValue();
        *(u16 *)(s + 0x372) = Script_GetValue();
    } else {
        *(u16 *)(s + 0x35C) = 0x28;
        *(u16 *)(s + 0x35E) = 0x32;
        *(u16 *)(s + 0x360) = 0x3C;
        *(u16 *)(s + 0x364) = 0x5A;
        *(u16 *)(s + 0x366) = 0x50;
        *(u16 *)(s + 0x368) = 0x3C;
        *(u16 *)(s + 0x36A) = 0x50;
        *(u16 *)(s + 0x36C) = 0x32;
        *(u16 *)(s + 0x36E) = 0x32;
        *(u16 *)(s + 0x370) = 0x2D;
        *(u16 *)(s + 0x372) = 0x1;
        *(u16 *)(s + 0x374) = 0xF;
    }

    SceneActors_Initialize(s, (u32)arg1, zeroKw, pos, r5);
    SceneBody_SetPositionOffset(s + 0x48, gUnk_0824DC78);
    Widget_InitLinked(s + 0x240, s + 0x220, 0, 0, 2, 0, 60, pos);

    zero = 0;
    SceneActors_StartUnmirroredAnimation(s + 0x240, s + 0x220, s + 0x220, 5);

    if (s[0xB3]) {
        mode = Script_GetKeywordValue(0x44, 0);
        switch (mode) {
            case 0:
                s[0xAC] = (u8)zeroKw;
                s[0xAD] = 0;
                s[0xAE] = (u8)(u32)arg1;
                *(u32 *)(s + 0xC4) = 0;
                s[0xB0] = (u8)(u32)arg1;
                s[0xAB] = (u8)(u32)arg1;
                *(void **)(s + 0x100) = (void *)SceneActorAnimation_UpdateDirectionalCallback;
                *(u32 *)(s + 0x90) = 0;
                s[0xB1] = (u8)(u32)arg1;
                break;
            case 1:
                s[0xAC] = 1;
                s[0xAD] = 0;
                s[0xAE] = 1;
                *(u32 *)(s + 0xC4) = 0;
                s[0xB0] = (u8)(u32)arg1;
                *(void **)(s + 0x100) = (void *)SceneActorAnimation_UpdateEightDirectionCallback;
                *(u32 *)(s + 0x90) = 0;
                s[0xB1] = (u8)(u32)arg1;
                break;
            case 2:
                s[0xAC] = 2;
                s[0xAD] = 0;
                s[0xAE] = 0;
                *(u32 *)(s + 0xC4) = 0;
                s[0xB0] = (u8)(u32)arg1;
                s[0xAB] = 0x1A;
                *(void **)(s + 0x100) = (void *)SceneActors_UpdateStationaryReaction;
                *(u32 *)(s + 0x90) = 0;
                s[0xB1] = (u8)(u32)arg1;
                break;
            default:
                s[0xAC] = 0;
                s[0xAD] = 0;
                s[0xAE] = 0;
                *(u32 *)(s + 0xC4) = 0;
                s[0xB0] = (u8)(u32)arg1;
                *(void **)(s + 0x100) = (void *)SceneActorAnimation_UpdateDirectionalCallback;
                *(u32 *)(s + 0x90) = 0;
                s[0xB1] = (u8)(u32)arg1;
                break;
        }
        s[0xA2] = 0;
        *(u32 *)(s + 0xA4) = 0;
    } else {
        s[0xAC] = 0;
        s[0xAD] = 0;
        s[0xAE] = 0;
        *(u32 *)(s + 0xC4) = 0;
        s[0xB0] = (u8)(u32)arg1;
        *(void **)(s + 0x100) = (void *)SceneActorAnimation_UpdateDirectionalCallback;
        *(u32 *)(s + 0x90) = 0;
        s[0xB1] = (u8)(u32)arg1;
        s[0xA2] = 0;
        *(u32 *)(s + 0xA4) = 0;
    }

    *(void **)(s + 0x104) = (void *)Vampire_ApplyPendingDamage;
    *(void **)(s + 0x108) = (void *)SceneActors_TryApplyRequestedState;
    SceneActors_SetHealthLimits(s, (u32)flags, (u32)flags, *(u16 *)(s + 0x176));
    SceneActorGraphics_InitializeBody(s);
    SceneActorGraphics_InitializeOverlay(s, *(u16 *)(s + 0x176));

    if (Script_SeekToKeyword(0x63)) {
        u32 a = Script_GetValue();
        u32 b = Script_GetValue();
        SceneActors_SetEventScripts(s, a, b);
        *(u32 *)(s + 0x388) = Script_GetValue();
    } else {
        SceneActors_SetEventScripts(s, 0, 0);
        *(u32 *)(s + 0x388) = 0;
    }

    if (Script_SeekToKeyword(0x41)) {
        for (i = 0; i <= 3 && Script_GetPc(); i++) {
            *(u32 *)(s + 0xF0 + i * 4) = Script_GetValue();
        }
    }

    if (Script_SeekToKeyword(0x43)) {
        *(u32 *)(s + 0x3BC) = Script_GetValue();
        *(u16 *)(s + 0x378) = Script_GetValue();
        *(u16 *)(s + 0x37A) = Script_GetValue();
    } else {
        *(u32 *)(s + 0x3C0) = 0x258;
        *(u16 *)(s + 0x378) = 0x32;
        *(u16 *)(s + 0x37A) = 0x32;
    }
    *(u32 *)(s + 0x3C0) = 0;
    *(u8 *)(s + 0x3B9) = 0;

    if (Script_SeekToKeyword(0x66)) {
        *(u16 *)(s + 0x37C) = Script_GetValue();
        *(u16 *)(s + 0x37E) = Script_GetValue();
    } else {
        *(u16 *)(s + 0x37C) = 0x1E;
        *(u16 *)(s + 0x37E) = 0x1E;
    }

    *(u16 *)(s + 0x380) = Script_GetKeywordValue(0x53, 0);
    *(u32 *)(s + 0x3AC) = 0;
    {
        u32 v4d = Script_GetKeywordValue(0x4D, 1);
        u32 bitA = (0 - v4d) | v4d;
        u32 r5b = bitA >> 31;
        *(u32 *)(s + 0x3AC) = r5b;
        if (Script_GetKeywordValue(0x54, 1))
            *(u32 *)(s + 0x3AC) = 2 | r5b;
        else
            *(u32 *)(s + 0x3AC) = r5b;
    }
    {
        u32 v = *(u32 *)(s + 0x3AC);
        if (Script_GetKeywordValue(0x48, 0))
            *(u32 *)(s + 0x3AC) = 4 | v;
        else
            *(u32 *)(s + 0x3AC) = v;
    }

    palette = gPaletteBankData;
    ScaledShadow_Initialize(s + 0x2A0, s + 0x260, 0x40, 0x40);
    ScaledShadow_SetGraphic(s + 0x2A0, 0x1C1B, 7);

    {
        u32 off = 0x66A0;
        dst = (u32 *)(s + 0x3C4);
        i = 15;
        do {
            CpuSet(palette + off, dst, 0x04000008);
            off += 0x20;
            dst += 8;
        } while ((s32)--i >= 0);
    }
    {
        u32 off = 0x6720;
        dst = (u32 *)(s + 0x5C4);
        i = 15;
        do {
            CpuSet(palette + off, dst, 0x04000008);
            dst += 8;
        } while ((s32)--i >= 0);
    }
    {
        u32 off = 0x6700;
        dst = (u32 *)(s + 0x7C4);
        i = 15;
        do {
            CpuSet(palette + off, dst, 0x04000008);
            dst += 8;
        } while ((s32)--i >= 0);
    }

    *(void **)(s + 0x288) = s + 0x3C4;
    s[0x38E] = 0;
    *(u32 *)(s + 0x390) = 0x335;
    s[0x38C] = 0;
    s[0x38F] = 5;

    *(void **)(s + 0x9DC) = BackgroundFlash_Create(*(s16 *)(s + 0x370), *(s16 *)(s + 0x36E));
    if (!*(void **)(s + 0x9DC))
        return -1;
    *(void **)(s + 0x9D4) = ContactSwarm_Create();
    if (!*(void **)(s + 0x9D4))
        return -1;
    *(void **)(s + 0x9D8) = ContactActorPool_Create(s[0x174], *(s16 *)(s + 0x36E), 0x64);
    if (!*(void **)(s + 0x9D8))
        return -1;

    s[0x394] = 1;
    s[0x3A2] = 4;
    CameraFollow_Initialize(s + 0x9E0, *(void **)(s + 0x110), s + 0x260, 1);
    *(u32 *)(s + 0x9E0 + 0x10) = r5;
    *(u32 *)(s + 0x9E0 + 0x14) = 0x8C;
    SceneActors_SetDamageCallback(s, s + 0x1CC, (void *)sub_08022B18, 0);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08028F78);
#endif
