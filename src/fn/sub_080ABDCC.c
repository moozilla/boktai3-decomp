#include "global.h"

#define TEN_AT(T, off) (*(T *)((u8 *)arg0 + (off)))

struct Ten32Block {
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
void TentacleActor_OnUpdate(void);
void TentacleActor_OnResolveVariantValue(void);
void TentacleBody_OnIdleState(void);
void TentacleBody_UpdateAttackWindup(void);
void TentacleBody_ResetDirectionalPose(void);
void sub_080A71A0(void);
void TentacleBody_UpdateDefeat(void);
void TentacleBody_UpdateHiddenTransition(void);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolveDamage(void);
void SceneActorReaction_SinkAndRemove(void);
void sub_080ABB18(void);
void ActorTrail_ResetAtSavedPosition(void);
void SceneActor_ResetQueuedState(void);
void SceneActor_TryRequestFacingReaction(void);
void sub_080AA744(void);
void SceneActorScript_OnCommand(void);
void TentacleEnemy_InitPlayerReference(void);
void TentacleBody_Destroy(void);
extern const u8 gUnk_086108B0[];
void TentacleActor_InitProfile(void *);
void TentacleActor_ResetRouteHistory(void *);
void TentacleActor_InitCollision(void *);
void TentacleActor_InitSegmentUpdateState(void *);
void TentacleActor_SelectWanderRoute(void *);
void TentacleBody_InitBehaviorTable(void *);
void TentacleActor_InitStateHandlers(void *);
void TentacleBody_InitSegmentAnimations(void *, s32, u32, u32);
void TentacleActor_ResetSegmentPositions(void *, void *);
void TentacleActor_InitSegmentColliders(void *);
void TentacleActor_ActivateMarkedSegment(void *);
void *TerrainModifiers_FindCellOverride(s32, s32);
s32 Div(s32, s32);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
void AnimData_SelectPaletteBank(void *);
s32 SceneActorScript_GetManagerField62(void);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
s32 SceneActorRegistry_LinkActor(void *);
void *Region_FindContaining(void *);

#ifdef NONMATCHING
// fuzzy: 30.4%
// scope shape 1272 (differences once low registers are set aside)
// confidence=low: tentacle enemy loader: script-driven parameters, animation, callback tables
s32 sub_080ABDCC(void *arg0)
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
    TEN_AT(u16, 0x8D0) = 0;
    SceneActorRegistry_SetObjectContext(arg0, arg0, 0);
    TEN_AT(u8, 0x98) = 9;
    TentacleActor_InitProfile(arg0);

    if (TEN_AT(u8, 8) == 0) {
        heap = Heap_Alloc(0x5C);
        TEN_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x5C);
        heap = TEN_AT(u8 *, 4);
        AnimData_Load(heap + 0x2C, 0xD636);
        Sprite_Init(heap, heap + 0x2C, 0);
        SceneActorAnimation_InitEnemySequence(heap, 0xC102);
        if (TEN_AT(u8, 0x98) == 0x11 || TEN_AT(u8, 0x98) == 0x17)
            heap[7] = 1;
        TEN_AT(u8, 0x59) = 0;
        flagp = (u8 *)arg0 + 0x59;
    } else {
        heap = Heap_Alloc(0x80);
        TEN_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x80);
        heap = TEN_AT(u8 *, 4);
        CpuSet(&zero, tmp, 0x05000002);
        res = ScriptProc_Start(0xCB05, 0xD636);
        *(struct Ten32Block *)heap = *(struct Ten32Block *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, tmp);
        TEN_AT(u8, 0x2B7) = 0xFF;
        flagp = (u8 *)arg0 + 0x59;
    }

    TEN_AT(u32, 0x2CC) = 0;
    TEN_AT(u32, 0x2D0) = 0;
    TEN_AT(u8, 0x2B4) = 0;
    TEN_AT(u16, 0x2E8) = 8;
    TEN_AT(u16, 0x2C0) = Script_GetKeywordValue(0x4D, 0);
    CpuSet(&zero, tmp2, 0x05000002);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        for (i = 0; i <= 3; i++) {
            if (Script_GetPc())
                r1v = Script_GetValue();
            else
                r1v = 0x18;
            TEN_AT(u16, 0xB4 + 2 * i) = r1v;
            if (Script_GetPc())
                r2v = Script_GetValue();
            else
                r2v = 0x18;
            TEN_AT(u16, 0xBC + 2 * i) = r2v;
            if (Script_GetPc())
                r3v = Script_GetValue();
            else
                r3v = 0;
            TEN_AT(u32, 0xCC + 4 * i) = r3v;
            if (Script_GetPc())
                r4v = Script_GetValue();
            else
                r4v = 0;
            TEN_AT(u16, 0xC4 + 2 * i) = r4v;
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
    TEN_AT(u32, 0xDC) = 0;
    TEN_AT(u16, 0xEC) = r4v;
    TEN_AT(u16, 0xEE) = r1v;
    TEN_AT(u8, 0xEA) = r2v;
    TEN_AT(u32, 0xF8) = r4v * r4v;
    TEN_AT(u32, 0xFC) = r1v * r1v;
    if (TEN_AT(u8, 0xEB) & 4)
        TEN_AT(u32, 0xF4) = TEN_AT(u32, 0xF8);
    else
        TEN_AT(u32, 0xF4) = Div(TEN_AT(u32, 0xF8) * 8 - TEN_AT(u32, 0xF8), 10);

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
        TEN_AT(u32, 0x2D0) |= 0x200;
    TEN_AT(u32, 0x1F0) = (u16)Script_GetKeywordValue(0x46, 0);
    TEN_AT(u32, 0x1EC) = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        TEN_AT(u16, 0xB0) = Script_GetPc() ? Script_GetValue() : 0;
        TEN_AT(u8, 0x9E) = Script_GetPc() ? Script_GetValue() : 0;
        TEN_AT(u8, 0x9F) = Script_GetPc() ? Script_GetValue() : 0;
        TEN_AT(u8, 0xA0) = Script_GetPc() ? Script_GetValue() : 0;
        TEN_AT(u16, 0xA2) = Script_GetPc() ? Script_GetValue() : 0;
        TEN_AT(u16, 0xA4) = Script_GetPc() ? Script_GetValue() : 0;
    }

    r1v = TEN_AT(u16, 0xB0);
    r3v = r4v;
    TEN_AT(u16, 0x2D4) = 0;
    TEN_AT(void *, 0x21C) = SceneActorControl_OnIdle;
    TEN_AT(u8, 0x2B2) = 1;
    TEN_AT(u8, 0x2AC) = 0;
    if ((s16)r1v <= 0)
        r1v = 0;
    TEN_AT(u16, 0x9C) = r1v;
    TEN_AT(u16, 0xAA) = 0;
    if (r3v <= 0)
        r3v = r6v;
    TEN_AT(u16, 0xA6) = r6v;
    TEN_AT(u16, 0xA8) = r3v;
    TEN_AT(u16, 0x2FA) = 0;

    if (Script_SeekToKeyword(0x58)) {
        TEN_AT(u32, 0xAC) = Script_GetPc() ? Script_GetValue() : 0;
        TEN_AT(u8, 0xA1) = Script_GetPc() ? Script_GetValue() : 0;
    }
    TEN_AT(u16, 0xB2) = Script_GetKeywordValue(0x4B, 2);
    r1v = Script_GetKeywordValue(0x56, -1);
    if ((s16)r1v < 0)
        r1v = 0xFFFF;
    TEN_AT(u16, 0x9A) = r1v;
    if (Script_GetKeywordValue(0x48, 0))
        TEN_AT(u32, 0x2D0) |= 0x800;

    SceneBody_Init(arg0 + 0x50, mode, tmp2, 0, 4, arg0);
    SceneBody_SetSprite(arg0 + 0x50, TEN_AT(u8, 8) ? 0 : TEN_AT(u8 *, 4));
    SceneBody_SetMovementProbe(arg0 + 0x50, arg0 + 0x40, 0x40, 0x40);

    if (Script_SeekToKeyword(0x70)) {
        TEN_AT(u16, 0x5C) = Script_GetPc() ? Script_GetValue() : 0;
        TEN_AT(u16, 0x5E) = Script_GetPc() ? Script_GetValue() : 0;
        TEN_AT(u16, 0x60) = Script_GetPc() ? Script_GetValue() : 0;
        x = (s8)(TEN_AT(u16, 0x5C) >> 8);
        y = (s8)(TEN_AT(u16, 0x60) >> 8);
        if (x >= 0 && y >= 0 && (u32)x < *(u32 *)0x030052F8 && (u32)y < *(u32 *)0x030052FC)
            r1v = *(u16 *)(*(u8 **)0x030052F4 + 0x24 + 2 * y) + x;
        else
            r1v = 0;
        TEN_AT(u16, 0x2F4) = r1v;
        r1v = TEN_AT(u16, 0x2F4);
        cell = TerrainModifiers_FindCellOverride(r1v, 1);
        if (cell == 0)
            cellp = (u8 *)(*(u32 *)(*(u8 **)0x030052F4 + 4) + (r1v << 2) + 0xC);
        else
            cellp = (u8 *)cell + 4;
        r2v = *cellp >> 4;
        r1v = (*cellp & 0xF) << 8;
        if (r2v == 1)
            r1v -= TEN_AT(u8, 0x5C + 4);
        else if (r2v == 2)
            r1v -= TEN_AT(u8, 0x5C);
        TEN_AT(u16, 0x2BE) = r1v;
        TEN_AT(u16, 0x2EA) = TEN_AT(u16, 0x5E) - r1v;
        TEN_AT(u16, 0x5E) = TEN_AT(u16, 0x2BE);
        if (TEN_AT(u8, 8) == 0) {
            *(u32 *)(TEN_AT(u8 *, 4) + 0x1C) = TEN_AT(u32, 0x5C);
            *(u32 *)(TEN_AT(u8 *, 4) + 0x20) = TEN_AT(u32, 0x60);
        } else {
            *(u32 *)(TEN_AT(u8 *, 4) + 0x40) = TEN_AT(u32, 0x5C);
            *(u32 *)(TEN_AT(u8 *, 4) + 0x44) = TEN_AT(u32, 0x60);
        }
        TEN_AT(u32, 0x92C) = TEN_AT(u32, 0x5C);
        TEN_AT(u32, 0x930) = TEN_AT(u32, 0x60);
    }

    if (Script_SeekToKeyword(0x50))
        TEN_AT(u16, 0x8E6) = Script_GetPc() ? Script_GetValue() : 0x96;
    TentacleActor_ResetRouteHistory(arg0);

    TEN_AT(u16, 0x1E6) = Script_GetKeywordValue(0x43, 0);
    TEN_AT(u16, 0x1E0) = 0x22A;
    TEN_AT(u16, 0x1E2) = 0xC9;
    TEN_AT(u16, 0x1E4) = TEN_AT(u16, 0x1E6) + 0x22A;
    r1v = TEN_AT(u16, 0x1E0) + TEN_AT(u16, 0x1E6);
    if (TEN_AT(u8, 8) == 0)
        AnimData_SelectPaletteBank(TEN_AT(u8 *, 4) + 0x2C);
    else
        *(u32 *)(TEN_AT(u8 *, 4) + 0x68) = *(u32 *)0x030042E4 + (r1v << 5);
    if (TEN_AT(u8, 8) == 1) {
        r1v = SceneActorScript_GetManagerField62();
        TEN_AT(u16, 0x5A) = r1v - 0x5FFF;
    }

    TentacleActor_InitCollision(arg0);
    TentacleActor_InitSegmentUpdateState(arg0);
    TEN_AT(u32, 0x2CC) |= 0x2000;
    TEN_AT(void *, 0x274) = TentacleActor_OnUpdate;
    TEN_AT(void *, 0x260) = TentacleActor_OnResolveVariantValue;
    TEN_AT(void *, 0x224) = TentacleBody_OnIdleState;
    TEN_AT(void *, 0x228) = TentacleBody_UpdateAttackWindup;
    TEN_AT(void *, 0x22C) = TentacleBody_ResetDirectionalPose;
    TEN_AT(void *, 0x230) = sub_080A71A0;
    TEN_AT(void *, 0x234) = TentacleBody_UpdateDefeat;
    TEN_AT(void *, 0x220) = TentacleBody_UpdateHiddenTransition;
    TEN_AT(void *, 0x278) = SceneActorReaction_OnEnemyDefault;
    TEN_AT(u32, 0x270) = 0;
    TEN_AT(void *, 0x250) = (void *)gUnk_086108B0;
    TEN_AT(u32, 0x248) = 0;
    TEN_AT(u32, 0x26C) = 0;
    TEN_AT(void *, 0x238) = SceneActorReaction_SinkAndRemove;
    TEN_AT(void *, 0x264) = SceneActorReaction_ResolveDamage;
    TEN_AT(void *, 0x268) = sub_080ABB18;
    TEN_AT(u32, 0x240) = 0;
    TEN_AT(u32, 0x104) = 0;
    TEN_AT(u32, 0x108) = 0;
    TEN_AT(u32, 0x10C) = 0;
    TEN_AT(u32, 0x110) = 0;
    TEN_AT(u32, 0x114) = 0;
    TEN_AT(void *, 0x248) = ActorTrail_ResetAtSavedPosition;
    TEN_AT(void *, 0x23C) = SceneActor_ResetQueuedState;
    TEN_AT(void *, 0x270) = SceneActor_TryRequestFacingReaction;
    TEN_AT(void *, 0x240) = sub_080AA744;
    TEN_AT(u8, 0xDC + 0xF) |= 1;
    TEN_AT(u32, 0x2CC) |= 0x200;
    TEN_AT(u32, 0x2D0) |= 8;
    TEN_AT(u16, 0x2C0) |= 0x40;
    SceneActorBehavior_InitializeShadow(arg0);
    SceneActorTerrain_ReadScriptParameters(arg0);
    flag = (TEN_AT(u16, 0x2C0) & 4) != 0;
    if (flag)
        TEN_AT(u32, 0x2D0) |= 0x2000;

    if (!flag) {
        TEN_AT(u8, 0x2A8) = 0;
        TEN_AT(u8, 0x2A9) = 0;
        TEN_AT(u8, 0x2AA) = 0;
        TEN_AT(u32, 0x2AC) = 0;
        TEN_AT(u8, 0x2B1) = 1;
        w0 = TEN_AT(u32, 0x22C);
        TEN_AT(u8, 0x2B2) = 1;
        TEN_AT(u8, 0x2B0) = 0;
        TEN_AT(u8, 0x2B5) = 0;
        TEN_AT(u32, 0x21C) = w0;
        TEN_AT(u16, 0x2C8) = 0;
        TEN_AT(u32, 0x114) &= ~1;
        TEN_AT(u32, 0x104) = 0;
    }

    if (TEN_AT(u16, 0x2C0) & 2)
        TEN_AT(u16, 0x2FA) = *(u32 *)(*(u8 **)0x02000710 + 0x5A0);
    else
        TEN_AT(u16, 0x2FA) = (u16)(u32)Region_FindContaining(arg0 + 0x5C);

    if (TEN_AT(u16, 0x2C0) & 0x40)
        TEN_AT(u16, 0x13A) &= 0xFEFF;

    if (Script_GetKeywordValue(0x53, 0) != 0 ||
        *(u32 *)(*(u8 **)0x02000710 + 0x5A0) != TEN_AT(u16, 0x2FA))
        SceneActorPlacement_HideActor(arg0, 0);

    SceneActorAnimation_SelectDirectionalEntry(arg0, 0);
    if (TEN_AT(u8, 8) != 0) {
        Anim_Step(TEN_AT(u8 *, 4) + 0x20);
    } else {
        u8 *h = TEN_AT(u8 *, 4);
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

    TEN_AT(u32, 0x304) = 0;
    TentacleActor_SelectWanderRoute(arg0);
    TEN_AT(u8, 0x2A8) = 0;
    TEN_AT(u8, 0x2A9) = 0;
    TEN_AT(u8, 0x2AA) = 2;
    TEN_AT(u32, 0x2AC) = 0;
    TEN_AT(u8, 0x2B1) = 1;
    w0 = TEN_AT(u32, 0x228);
    TEN_AT(u8, 0x2B2) = 1;
    TEN_AT(u8, 0x2B0) = 0;
    TEN_AT(u8, 0x2B5) = 1;
    TEN_AT(u32, 0x21C) = w0;
    TEN_AT(u16, 0x2C8) = 0;
    TEN_AT(u32, 0x114) &= ~1;
    TEN_AT(u32, 0x104) = 0;
    SceneActorBehavior_InstallCallbackTables(arg0);
    TentacleBody_InitBehaviorTable(arg0);
    TentacleActor_InitStateHandlers(arg0);
    TentacleBody_InitSegmentAnimations(arg0, 0xD636, 0xC102, *flagp);
    TentacleActor_ResetSegmentPositions(arg0, arg0 + 0x5C);
    TentacleActor_InitSegmentColliders(arg0);
    TentacleActor_SelectWanderRoute(arg0);
    TentacleActor_ActivateMarkedSegment(arg0);
    TEN_AT(u16, 0x12E) = TEN_AT(u16, 0x2C0);
    TEN_AT(u16, 0x12C) = TEN_AT(u16, 0x2EA);
    TEN_AT(u32, 0x118) = TEN_AT(u32, 0x2CC);
    TEN_AT(u32, 0x11C) = TEN_AT(u32, 0x2D0);
    TEN_AT(u8, 0x120) = *flagp;
    TEN_AT(u8, 0x121) = TEN_AT(u8, 0x2A8);
    TEN_AT(u8, 0x122) = TEN_AT(u8, 0x2A9);
    TEN_AT(u8, 0x123) = TEN_AT(u8, 0x2AA);
    TEN_AT(u32, 0x124) = TEN_AT(u32, 0x21C);
    TEN_AT(u8, 0x128) = TEN_AT(u8, 0x2B5);
    TEN_AT(void *, 0x24C) = SceneActorScript_OnCommand;
    TEN_AT(void *, 0x258) = TentacleEnemy_InitPlayerReference;
    TEN_AT(void *, 0x25C) = TentacleBody_Destroy;

    return SceneActorRegistry_LinkActor(arg0) ? 0 : -1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080ABDCC);
#endif
