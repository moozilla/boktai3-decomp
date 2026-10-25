#include "global.h"
extern const u8 gUnk_08605DB8[];

#define BEE_AT(T, off) (*(T *)((u8 *)arg0 + (off)))

struct Bee32Block {
    u32 w[8];
};

s32 Script_SeekToKeyword(u32);
s32 Script_GetKeywordValue(u32, s32);
s32 Script_GetValue(void);
u32 Script_GetPc(void);
void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneActorAnimation_InitEnemySequence(void *, u32);
void CpuSet(const void *, void *, u32);
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void SceneActorRegistry_SetObjectContext(void *, void *, s32);
void SceneActorControl_OnIdle(void);
void Bee_OnUpdate(void);
void sub_08093094(void);
void Bee_UpdateTurnAnimation(void);
void Bee_ResetIdleAnimation(void);
void Bee_UpdateMovementStop(void);
void Bee_UpdateDefeat(void);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolveDamage(void);
void SceneActorPlacement_PlayResponseAndResetState(void);
void SceneActorReaction_SinkAndRemove(void);
void Slime_SetStateFlag(void);
void Bee_ResetRoute(void);
void SceneActorScript_OnCommand(void);
void Bee_OnPlayerInteraction(void);
void Bee_Destroy(void);
void Bee_OnDetectionCheck(void);
void Bee_InitProfile(void *);
void *TerrainModifiers_FindCellOverride(s32, s32);
s32 Div(s32, s32);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
void AnimData_SelectPaletteBank(void *);
s32 SceneActorScript_GetManagerField62(void);
void Bee_InitColliders(void *);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void Bee_InitActionPreset(void *);
void Bee_InitBehaviorHandlers(void *);
void Bee_InitReactionHandlers(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
s32 SceneActorRegistry_LinkActor(void *);
void *Region_FindContaining(void *);

#ifdef NONMATCHING
// fuzzy: 24.1%
// scope shape 1523 (differences once low registers are set aside)
// confidence=low: bee actor setup: script parameters, animation tables, callbacks
s32 Bee_Init(void *arg0)
{
    u8 *flagp;
    u8 *heap;
    u32 zero;
    u32 tmp[2];
    u32 tmp2[2];
    void *res;
    s32 mode;
    s32 i;
    u32 flag;
    s32 r4v, r6v, r3v, r2v, r1v;
    u32 w0, w1;
    s32 x, y;
    u8 *cellp;
    void *cell;

    zero = 0;
    BEE_AT(u32, 0x3D4) = 0;
    SceneActorRegistry_SetObjectContext(arg0, arg0, 0);
    BEE_AT(u8, 0x98) = 0x14;
    Bee_InitProfile(arg0);

    if (BEE_AT(u8, 8) == 0) {
        heap = Heap_Alloc(0x5C);
        BEE_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x5C);
        heap = BEE_AT(u8 *, 4);
        AnimData_Load(heap + 0x2C, 0x9506);
        Sprite_Init(heap, heap + 0x2C, 0);
        SceneActorAnimation_InitEnemySequence(heap, 0x62C7);
        if (BEE_AT(u8, 0x98) == 0x11 || BEE_AT(u8, 0x98) == 0x17)
            heap[7] = 1;
        BEE_AT(u8, 0x59) = 0;
        flagp = (u8 *)arg0 + 0x59;
    } else {
        heap = Heap_Alloc(0x80);
        BEE_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x80);
        heap = BEE_AT(u8 *, 4);
        CpuSet(&zero, tmp, 0x05000002);
        res = ScriptProc_Start(0xCB05, 0x9506);
        *(struct Bee32Block *)heap = *(struct Bee32Block *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, tmp);
        BEE_AT(u8, 0x2B7) = 0xFF;
        flagp = (u8 *)arg0 + 0x59;
    }

    BEE_AT(u8, 0x2CC) = 0;
    BEE_AT(u32, 0x2CC) = 0;
    BEE_AT(u32, 0x2D0) = 0;
    BEE_AT(u8, 0x2B4) = 0;
    BEE_AT(u16, 0x2E8) = 8;
    BEE_AT(u16, 0x2C0) = Script_GetKeywordValue(0x4D, 0);
    CpuSet(&zero, tmp2, 0x05000002);
    mode = Script_GetKeywordValue(0x4E, 0);
    BEE_AT(u32, 0x2D8) = 0;
    tmp2[0] = mode;

    if (Script_SeekToKeyword(0x61)) {
        for (i = 0; i <= 3; i++) {
            if (Script_GetPc())
                r1v = Script_GetValue();
            else
                r1v = 0x18;
            BEE_AT(u16, 0xB4 + 2 * i) = r1v;
            if (Script_GetPc())
                r2v = Script_GetValue();
            else
                r2v = 0x18;
            BEE_AT(u16, 0xBC + 2 * i) = r2v;
            if (Script_GetPc())
                r3v = Script_GetValue();
            else
                r3v = 0;
            BEE_AT(u32, 0xCC + 4 * i) = r3v;
            if (Script_GetPc())
                r4v = Script_GetValue();
            else
                r4v = 0;
            BEE_AT(u16, 0xC4 + 2 * i) = r4v;
        }
    }

    if (Script_SeekToKeyword(0x65)) {
        if (Script_GetPc())
            r4v = Script_GetValue();
        else
            r4v = 0x30;
        if (Script_GetPc())
            r2v = Script_GetValue() & 0xFF;
        else
            r2v = 0x40;
        r1v = r4v;
    } else {
        r4v = 0x30;
        r1v = 0x30;
        r2v = 0x40;
    }
    BEE_AT(u32, 0xDC) = 0;
    BEE_AT(u16, 0xEC) = r4v;
    BEE_AT(u16, 0xEE) = r1v;
    BEE_AT(u8, 0xEA) = r2v;
    BEE_AT(u32, 0xF8) = r4v * r4v;
    BEE_AT(u32, 0xFC) = r1v * r1v;
    if (BEE_AT(u8, 0xEB) & 4)
        BEE_AT(u32, 0xF4) = BEE_AT(u32, 0xF8);
    else
        BEE_AT(u32, 0xF4) = Div(BEE_AT(u32, 0xF8) * 8 - BEE_AT(u32, 0xF8), 10);

    if (Script_SeekToKeyword(0x6C)) {
        if (Script_GetPc())
            r6v = Script_GetValue();
        else
            r6v = 0x12C;
        if (Script_GetPc())
            r4v = Script_GetValue();
        else
            r4v = 0x12C;
    } else {
        r6v = 0;
        r4v = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        BEE_AT(u32, 0x2D0) |= 0x200;
    BEE_AT(u32, 0x1F0) = (u16)Script_GetKeywordValue(0x46, 0);
    BEE_AT(u32, 0x1EC) = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        BEE_AT(u16, 0xB0) = Script_GetPc() ? Script_GetValue() : 0;
        BEE_AT(u8, 0x9E) = Script_GetPc() ? Script_GetValue() : 0;
        BEE_AT(u8, 0x9F) = Script_GetPc() ? Script_GetValue() : 0;
        BEE_AT(u8, 0xA0) = Script_GetPc() ? Script_GetValue() : 0;
        BEE_AT(u16, 0xA2) = Script_GetPc() ? Script_GetValue() : 0;
        BEE_AT(u16, 0xA4) = Script_GetPc() ? Script_GetValue() : 0;
    }

    r1v = BEE_AT(u16, 0xB0);
    r3v = r4v;
    BEE_AT(u16, 0x2D4) = 0;
    BEE_AT(void *, 0x21C) = SceneActorControl_OnIdle;
    BEE_AT(u8, 0x2B2) = 1;
    BEE_AT(u8, 0x2AC) = 0;
    if ((s16)r1v <= 0)
        r1v = 0;
    BEE_AT(u16, 0x9C) = r1v;
    BEE_AT(u16, 0xAA) = 0;
    if (r3v <= 0)
        r3v = r6v;
    BEE_AT(u16, 0xA6) = r6v;
    BEE_AT(u16, 0xA8) = r3v;
    BEE_AT(u16, 0x2FA) = 0;

    if (Script_SeekToKeyword(0x58)) {
        BEE_AT(u32, 0xAC) = Script_GetPc() ? Script_GetValue() : 0;
        BEE_AT(u8, 0xA1) = Script_GetPc() ? Script_GetValue() : 0;
    }
    BEE_AT(u16, 0xB2) = Script_GetKeywordValue(0x4B, 2);
    r1v = Script_GetKeywordValue(0x56, -1);
    if ((s16)r1v < 0)
        r1v = 0xFFFF;
    BEE_AT(u16, 0x2F8) = r1v;
    if (Script_GetKeywordValue(0x48, 0))
        BEE_AT(u32, 0x2D0) |= 0x800;

    SceneBody_Init(arg0 + 0x50, tmp2[0], tmp2, 0, 4, arg0);
    SceneBody_SetSprite(arg0 + 0x50, BEE_AT(u8, 8) ? 0 : BEE_AT(u8 *, 4));
    SceneBody_SetMovementProbe(arg0 + 0x50, arg0 + 0x40, 0x40, 0x40);

    if (Script_SeekToKeyword(0x50)) {
        r2v = Script_GetPc() ? Script_GetValue() : 0x68;
        BEE_AT(u8, 0x429) = r2v;
        BEE_AT(u8, 0xF7) = 0;
        BEE_AT(u16, 0x42C) = Script_GetPc() ? Script_GetValue() : 0x64;
    }

    if (Script_SeekToKeyword(0x70)) {
        BEE_AT(u16, 0x5C) = Script_GetPc() ? Script_GetValue() : 0;
        BEE_AT(u16, 0x5E) = Script_GetPc() ? Script_GetValue() : 0;
        BEE_AT(u16, 0x60) = Script_GetPc() ? Script_GetValue() : 0;
        w0 = BEE_AT(u32, 0x5C);
        w1 = BEE_AT(u32, 0x60);
        if (*flagp == 0)
            w0 = (w0 & 0xFFFF0000) | (u16)((u16)w0 + 0x80);
        else
            w1 = (w1 & 0xFFFF0000) | (u16)((u16)w1 + 0x80);
        BEE_AT(u32, 0x488) = w0;
        BEE_AT(u32, 0x48C) = w1;
        x = (s8)((u16)w0 >> 8);
        y = (s8)((u16)w1 >> 8);
        if (x >= 0 && y >= 0 && (u32)x < *(u32 *)0x030052F8 && (u32)y < *(u32 *)0x030052FC)
            r1v = *(u16 *)(*(u8 **)0x030052F4 + 0x24 + 2 * y) + x;
        else
            r1v = 0;
        BEE_AT(u16, 0x2F4) = r1v;
        r1v = BEE_AT(u16, 0x2F4);
        cell = TerrainModifiers_FindCellOverride(r1v, 1);
        if (cell == 0)
            cellp = (u8 *)(*(u32 *)(*(u8 **)0x030052F4 + 4) + (r1v << 2) + 0xC);
        else
            cellp = (u8 *)cell + 4;
        r2v = *cellp >> 4;
        r1v = (*cellp & 0xF) << 8;
        if (r2v == 1)
            r1v -= BEE_AT(u8, 0x488 + 4);
        else if (r2v == 2)
            r1v -= BEE_AT(u8, 0x488);
        BEE_AT(u16, 0x2BE) = r1v;
        BEE_AT(u16, 0x2EA) = BEE_AT(u16, 0x5E) - r1v;
        BEE_AT(u16, 0x3D8) = BEE_AT(u16, 0x2EA);
        BEE_AT(u16, 0x5E) = BEE_AT(u16, 0x2BE);
        BEE_AT(u16, 0x3DA) = BEE_AT(u16, 0x2BE);
    }

    BEE_AT(u32, 0x440) = 0;
    BEE_AT(u16, 0x178) = Script_GetKeywordValue(0x6E, 0);
    BEE_AT(u16, 0x1E6) = Script_GetKeywordValue(0x43, 0);
    BEE_AT(u16, 0x1E0) = 0xF4;
    BEE_AT(u16, 0x1E2) = 0x192;
    BEE_AT(u16, 0x1E4) = BEE_AT(u16, 0x1E6) + 0xF4;
    if (BEE_AT(u8, 8) == 0)
        AnimData_SelectPaletteBank(BEE_AT(u8 *, 4) + 0x2C);
    else
        BEE_AT(u8 *, 4)[0x68] = 0;
    if (BEE_AT(u8, 8) == 1) {
        r1v = SceneActorScript_GetManagerField62();
        BEE_AT(u16, 0x5A) = r1v - 0x5FFF;
    }

    Bee_InitColliders(arg0);
    BEE_AT(void *, 0x274) = Bee_OnUpdate;
    BEE_AT(void *, 0x260) = sub_08093094;
    BEE_AT(u32, 0x220) = 0;
    BEE_AT(void *, 0x228) = Bee_UpdateTurnAnimation;
    BEE_AT(u32, 0x224) = 0;
    BEE_AT(void *, 0x22C) = Bee_ResetIdleAnimation;
    BEE_AT(void *, 0x230) = Bee_UpdateMovementStop;
    BEE_AT(void *, 0x234) = Bee_UpdateDefeat;
    BEE_AT(void *, 0x278) = SceneActorReaction_OnEnemyDefault;
    BEE_AT(void *, 0x264) = SceneActorReaction_ResolveDamage;
    BEE_AT(void *, 0x268) = SceneActorPlacement_PlayResponseAndResetState;
    BEE_AT(void *, 0x238) = SceneActorReaction_SinkAndRemove;
    BEE_AT(void *, 0x23C) = Slime_SetStateFlag;
    BEE_AT(u32, 0x2CC) |= 0x2000;
    BEE_AT(void *, 0x248) = Bee_ResetRoute;
    BEE_AT(void *, 0x24C) = SceneActorScript_OnCommand;
    BEE_AT(u32, 0x250) = (u32)gUnk_08605DB8;
    BEE_AT(void *, 0x258) = Bee_OnPlayerInteraction;
    BEE_AT(void *, 0x25C) = Bee_Destroy;
    BEE_AT(void *, 0x270) = Bee_OnDetectionCheck;
    BEE_AT(u32, 0x2D0) |= 0x02100408;
    BEE_AT(u16, 0x2C0) |= 0x60;
    SceneActorBehavior_InitializeShadow(arg0);
    SceneActorTerrain_ReadScriptParameters(arg0);
    flag = (BEE_AT(u16, 0x2C0) & 4) != 0;
    if (flag)
        BEE_AT(u32, 0x2D0) |= 0x2000;

    if (!flag) {
        BEE_AT(u8, 0x2A8) = 0;
        BEE_AT(u8, 0x2A9) = 0;
        BEE_AT(u8, 0x2AA) = 0;
        BEE_AT(u32, 0x2AC) = 0;
        BEE_AT(u8, 0x2B1) = 1;
        BEE_AT(u8, 0x2B2) = 1;
        BEE_AT(u8, 0x2AC) = 0;
        BEE_AT(u8, 0x2B5) = 0;
        BEE_AT(u32, 0x21C) = BEE_AT(u32, 0x22C);
        BEE_AT(u16, 0x2C8) = 0;
        BEE_AT(u32, 0x114) &= ~1;
        BEE_AT(u32, 0x104) = 0;
    }

    if (BEE_AT(u16, 0x2C0) & 2)
        BEE_AT(u16, 0x2FA) = *(u32 *)(*(u8 **)0x02000710 + 0x5A0);
    else
        BEE_AT(u16, 0x2FA) = (u16)(u32)Region_FindContaining(arg0 + 0x5C);

    if (BEE_AT(u16, 0x2C0) & 0x40)
        BEE_AT(u16, 0x13A) &= 0xFEFF;

    if (Script_GetKeywordValue(0x53, 0) != 0 ||
        *(u32 *)(*(u8 **)0x02000710 + 0x5A0) != BEE_AT(u16, 0x2FA))
        SceneActorPlacement_HideActor(arg0, 0);

    SceneActorAnimation_SelectDirectionalEntry(arg0, 0);
    if (BEE_AT(u8, 8) != 0) {
        Anim_Step(BEE_AT(u8 *, 4) + 0x20);
    } else {
        u8 *h = BEE_AT(u8 *, 4);
        u8 *a = h + 0x48;
        u16 *frame = (u16 *)(*(u8 **)(h + 0x48) + (*(u16 *)(a + 8) << 1));
        *(u16 *)(h + 0x10) = *frame >> 6;
        if ((a[4] & 1) != ((*frame >> 4) & 1))
            *(u32 *)h |= 4;
        else
            *(u32 *)h &= ~4;
        if ((a[4] & 2) != ((*frame >> 4) & 2))
            *(u32 *)h |= 8;
        else
            *(u32 *)h &= ~8;
        *(u16 *)(a + 0xE) += 1;
        if (*(u16 *)(a + 0xE) >= a[7]) {
            *(u16 *)(a + 0xE) = 0;
            if (a[4] & 4) {
                if (*(u16 *)(a + 8) == 0) {
                    if (a[4] & 8)
                        *(u16 *)(a + 0xE) = a[7];
                    else
                        *(u16 *)(a + 8) = a[5] - 1;
                } else {
                    *(u16 *)(a + 8) -= 1;
                }
            } else {
                *(u16 *)(a + 8) += 1;
                if (*(u16 *)(a + 8) >= a[5]) {
                    if (a[4] & 8)
                        *(u16 *)(a + 0xE) = a[7];
                    else
                        *(u16 *)(a + 8) = a[5] - 1;
                }
            }
        }
        frame = (u16 *)(*(u8 **)(h + 0x48) + (*(u16 *)(a + 8) << 1));
        a[6] = *frame & 0xF;
        a[7] = (a[6] * *(u16 *)(a + 0xC)) >> 6;
        if (a[7] == 0)
            a[7] = 1;
    }

    BEE_AT(u32, 0x304) = 0;
    BEE_AT(u8, 0x2A8) = 0;
    BEE_AT(u8, 0x2A9) = 2;
    BEE_AT(u8, 0x2AA) = 0;
    BEE_AT(u32, 0x2AC) = 0;
    BEE_AT(u8, 0x2B1) = 1;
    BEE_AT(u32, 0x21C) = BEE_AT(u32, 0x228);
    BEE_AT(u8, 0x2B2) = 1;
    BEE_AT(u8, 0x2AC) = 0;
    BEE_AT(u8, 0x2B5) = 1;
    BEE_AT(u16, 0x2C8) = 0;
    BEE_AT(u32, 0x114) &= ~1;
    BEE_AT(u32, 0x104) = 0;

    SceneActorBehavior_InstallCallbackTables(arg0);
    Bee_InitActionPreset(arg0);
    Bee_InitBehaviorHandlers(arg0);
    Bee_InitReactionHandlers(arg0);
    BEE_AT(u16, 0x2E8) = 2;
    BEE_AT(u8 *, 4)[7] = 1;
    BEE_AT(u16, 0x12E) = BEE_AT(u16, 0x2C0);
    BEE_AT(u16, 0x12C) = BEE_AT(u16, 0x2EA);
    BEE_AT(u32, 0x118) = BEE_AT(u32, 0x2CC);
    BEE_AT(u32, 0x11C) = BEE_AT(u32, 0x2D0);
    BEE_AT(u8, 0x120) = *flagp;
    BEE_AT(u8, 0x121) = BEE_AT(u8, 0x2A8);
    BEE_AT(u8, 0x122) = BEE_AT(u8, 0x2A9);
    BEE_AT(u8, 0x123) = BEE_AT(u8, 0x2AA);
    BEE_AT(u32, 0x124) = BEE_AT(u32, 0x21C);
    BEE_AT(u8, 0x128) = BEE_AT(u8, 0x2B5);

    return SceneActorRegistry_LinkActor(arg0) ? 0 : -1;
}
#else
INCLUDE_ASM("asm/nonmatching", Bee_Init);
#endif
