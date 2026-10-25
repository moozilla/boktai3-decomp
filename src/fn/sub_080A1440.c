#include "global.h"

// confidence=low: Crow enemy loader: script-driven parameters, animation, callback tables
#define CROW_AT(T, off) (*(T *)((u8 *)arg0 + (off)))

struct Crow32Block {
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
void Crow_Update(void);
void Crow_GetTimedAction(void);
void Crow_PlayEntry2(void);
void Crow_OnIdleAnimation(void);
void RouteActor_UpdatePeriodicAnimation(void);
void Crow_RecoverFromDisplacement(void);
void SceneActorControl_CompleteScriptedResponse_0809F368(void);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolveDamage(void);
void SceneActorPlacement_PlayResponseAndResetState(void);
void SceneActorReaction_SinkAndRemove(void);
void Crow_ResetGroundState(void);
void Crow_BeginReaction(void);
void Crow_TryActivatePlayerProximityResponse(void);
void SceneActorScript_OnCommand(void);
void Crow_InitPlayerTarget(void);
void Crow_Destroy(void);
void Crow_InitProfile(void *);
void Crow_ReadSpawnScript(void *);
void Crow_InitActionTable(void *);
void Crow_SetBehaviorTable(void *);
extern const u8 gUnk_08605EC4[];
void *TerrainModifiers_FindCellOverride(s32, s32);
s32 Div(s32, s32);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
void AnimData_SelectPaletteBank(void *);
s32 SceneActorScript_GetManagerField62(void);
void Crow_InitColliders(void *);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
s32 SceneActorRegistry_LinkActor(void *);
void *Region_FindContaining(void *);

#ifdef NONMATCHING
// fuzzy: 27.5%
// scope shape 1086 (differences once low registers are set aside)
// confidence=low: Crow enemy loader: script-driven parameters, animation, callback tables
s32 Crow_Init(void *arg0)
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
    CROW_AT(u32, 0x3D4) = 0;
    SceneActorRegistry_SetObjectContext(arg0, arg0, 0);
    CROW_AT(u8, 0x98) = 0x13;
    Crow_InitProfile(arg0);

    if (CROW_AT(u8, 8) == 0) {
        heap = Heap_Alloc(0x5C);
        CROW_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x5C);
        heap = CROW_AT(u8 *, 4);
        AnimData_Load(heap + 0x2C, 0x5A6E);
        Sprite_Init(heap, heap + 0x2C, 0);
        SceneActorAnimation_InitEnemySequence(heap, 0x7B03);
        if (CROW_AT(u8, 0x98) == 0x11 || CROW_AT(u8, 0x98) == 0x17)
            heap[7] = 1;
        CROW_AT(u8, 0x59) = 0;
        flagp = (u8 *)arg0 + 0x59;
    } else {
        heap = Heap_Alloc(0x80);
        CROW_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x80);
        heap = CROW_AT(u8 *, 4);
        CpuSet(&zero, tmp, 0x05000002);
        res = ScriptProc_Start(0xCB05, 0x5A6E);
        *(struct Crow32Block *)heap = *(struct Crow32Block *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, tmp);
        CROW_AT(u8, 0x2B7) = 0xFF;
        flagp = (u8 *)arg0 + 0x59;
    }

    CROW_AT(u32, 0x2CC) = 0;
    CROW_AT(u32, 0x2D0) = 0;
    CROW_AT(u8, 0x2B4) = 0;
    CROW_AT(u16, 0x2E8) = 8;
    CROW_AT(u16, 0x2C0) = Script_GetKeywordValue(0x4D, 0);
    CpuSet(&zero, tmp2, 0x05000002);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        for (i = 0; i <= 3; i++) {
            if (Script_GetPc())
                r1v = Script_GetValue();
            else
                r1v = 0x18;
            CROW_AT(u16, 0xB4 + 2 * i) = r1v;
            if (Script_GetPc())
                r2v = Script_GetValue();
            else
                r2v = 0x18;
            CROW_AT(u16, 0xBC + 2 * i) = r2v;
            if (Script_GetPc())
                r3v = Script_GetValue();
            else
                r3v = 0;
            CROW_AT(u32, 0xCC + 4 * i) = r3v;
            if (Script_GetPc())
                r4v = Script_GetValue();
            else
                r4v = 0;
            CROW_AT(u16, 0xC4 + 2 * i) = r4v;
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
    CROW_AT(u32, 0xDC) = 0;
    CROW_AT(u16, 0xEC) = r4v;
    CROW_AT(u16, 0xEE) = r1v;
    CROW_AT(u8, 0xEA) = r2v;
    CROW_AT(u32, 0xF8) = r4v * r4v;
    CROW_AT(u32, 0xFC) = r1v * r1v;
    if (CROW_AT(u8, 0xEB) & 4)
        CROW_AT(u32, 0xF4) = CROW_AT(u32, 0xF8);
    else
        CROW_AT(u32, 0xF4) = Div(CROW_AT(u32, 0xF8) * 8 - CROW_AT(u32, 0xF8), 10);

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
        CROW_AT(u32, 0x2D0) |= 0x200;
    CROW_AT(u32, 0x1F0) = (u16)Script_GetKeywordValue(0x46, 0);
    CROW_AT(u32, 0x1EC) = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        CROW_AT(u16, 0xB0) = Script_GetPc() ? Script_GetValue() : 0;
        CROW_AT(u8, 0x9E) = Script_GetPc() ? Script_GetValue() : 0;
        CROW_AT(u8, 0x9F) = Script_GetPc() ? Script_GetValue() : 0;
        CROW_AT(u8, 0xA0) = Script_GetPc() ? Script_GetValue() : 0;
        CROW_AT(u16, 0xA2) = Script_GetPc() ? Script_GetValue() : 0;
        CROW_AT(u16, 0xA4) = Script_GetPc() ? Script_GetValue() : 0;
    }

    r1v = CROW_AT(u16, 0xB0);
    r3v = r4v;
    CROW_AT(u16, 0x2D4) = 0;
    CROW_AT(void *, 0x21C) = SceneActorControl_OnIdle;
    CROW_AT(u8, 0x2B2) = 1;
    CROW_AT(u8, 0x2AC) = 0;
    if ((s16)r1v <= 0)
        r1v = 0;
    CROW_AT(u16, 0x9C) = r1v;
    CROW_AT(u16, 0xAA) = 0;
    if (r3v <= 0)
        r3v = r6v;
    CROW_AT(u16, 0xA6) = r6v;
    CROW_AT(u16, 0xA8) = r3v;
    CROW_AT(u16, 0x2FA) = 0;

    if (Script_SeekToKeyword(0x58)) {
        CROW_AT(u32, 0xAC) = Script_GetPc() ? Script_GetValue() : 0;
        CROW_AT(u8, 0xA1) = Script_GetPc() ? Script_GetValue() : 0;
    }
    CROW_AT(u16, 0xB2) = Script_GetKeywordValue(0x4B, 2);
    r1v = Script_GetKeywordValue(0x56, -1);
    if ((s16)r1v < 0)
        r1v = 0xFFFF;
    CROW_AT(u16, 0x2F8) = r1v;
    if (Script_GetKeywordValue(0x48, 0))
        CROW_AT(u32, 0x2D0) |= 0x800;

    SceneBody_Init(arg0 + 0x50, mode, tmp2, 0, 4, arg0);
    SceneBody_SetSprite(arg0 + 0x50, CROW_AT(u8, 8) ? 0 : CROW_AT(u8 *, 4));
    SceneBody_SetMovementProbe(arg0 + 0x50, arg0 + 0x40, 0x40, 0x40);

    Crow_ReadSpawnScript(arg0);

    CROW_AT(u16, 0x1E6) = Script_GetKeywordValue(0x43, 0);
    CROW_AT(u16, 0x1E0) = 0x1CB;
    CROW_AT(u16, 0x1E2) = 0x192;
    CROW_AT(u16, 0x1E4) = CROW_AT(u16, 0x1E6) + 0x1CE;
    r1v = CROW_AT(u16, 0x1E0) + CROW_AT(u16, 0x1E6);
    if (CROW_AT(u8, 8) == 0)
        AnimData_SelectPaletteBank(CROW_AT(u8 *, 4) + 0x2C);
    else
        *(u32 *)(CROW_AT(u8 *, 4) + 0x68) = *(u32 *)0x030042E4 + (r1v << 5);
    if (CROW_AT(u8, 8) == 1) {
        r1v = SceneActorScript_GetManagerField62();
        CROW_AT(u16, 0x5A) = r1v - 0x5FFF;
    }

    Crow_InitColliders(arg0);
    CROW_AT(u32, 0x2CC) |= 0x2000;
    CROW_AT(void *, 0x274) = Crow_Update;
    CROW_AT(void *, 0x260) = Crow_GetTimedAction;
    CROW_AT(void *, 0x224) = Crow_PlayEntry2;
    CROW_AT(void *, 0x228) = Crow_OnIdleAnimation;
    CROW_AT(void *, 0x22C) = RouteActor_UpdatePeriodicAnimation;
    CROW_AT(void *, 0x230) = Crow_RecoverFromDisplacement;
    CROW_AT(void *, 0x234) = SceneActorControl_CompleteScriptedResponse_0809F368;
    CROW_AT(u32, 0x220) = 0;
    CROW_AT(void *, 0x278) = SceneActorReaction_OnEnemyDefault;
    CROW_AT(u32, 0x270) = 0;
    CROW_AT(u32, 0x248) = 0;
    CROW_AT(u32, 0x26C) = 0;
    CROW_AT(void *, 0x238) = SceneActorReaction_SinkAndRemove;
    CROW_AT(void *, 0x264) = SceneActorReaction_ResolveDamage;
    CROW_AT(void *, 0x268) = SceneActorPlacement_PlayResponseAndResetState;
    CROW_AT(u32, 0x240) = 0;
    CROW_AT(u32, 0x104) = 0;
    CROW_AT(u32, 0x108) = 0;
    CROW_AT(u32, 0x10C) = 0;
    CROW_AT(u32, 0x110) = 0;
    CROW_AT(u32, 0x114) = 0;
    CROW_AT(void *, 0x250) = (void *)gUnk_08605EC4;
    CROW_AT(void *, 0x248) = Crow_ResetGroundState;
    CROW_AT(void *, 0x23C) = Crow_BeginReaction;
    CROW_AT(void *, 0x270) = Crow_TryActivatePlayerProximityResponse;
    CROW_AT(u16, 0x2C0) |= 0x60;
    SceneActorBehavior_InitializeShadow(arg0);
    SceneActorTerrain_ReadScriptParameters(arg0);
    flag = (CROW_AT(u16, 0x2C0) & 4) != 0;
    if (flag)
        CROW_AT(u32, 0x2D0) |= 0x2000;

    if (!flag) {
        CROW_AT(u8, 0x2A8) = 0;
        CROW_AT(u8, 0x2A9) = 0;
        CROW_AT(u8, 0x2AA) = 0;
        CROW_AT(u32, 0x2AC) = 0;
        CROW_AT(u8, 0x2B1) = 1;
        CROW_AT(u8, 0x2B2) = 1;
        CROW_AT(u8, 0x2AC) = 0;
        CROW_AT(u8, 0x2B5) = 0;
        CROW_AT(u32, 0x21C) = CROW_AT(u32, 0x22C);
        CROW_AT(u16, 0x2C8) = 0;
        CROW_AT(u32, 0x114) &= ~1;
        CROW_AT(u32, 0x104) = 0;
    }

    if (CROW_AT(u16, 0x2C0) & 2)
        CROW_AT(u16, 0x2FA) = *(u32 *)(*(u8 **)0x02000710 + 0x5A0);
    else
        CROW_AT(u16, 0x2FA) = (u16)(u32)Region_FindContaining(arg0 + 0x5C);

    if (CROW_AT(u16, 0x2C0) & 0x40)
        CROW_AT(u16, 0x13A) &= 0xFEFF;

    if (Script_GetKeywordValue(0x53, 0) != 0 ||
        *(u32 *)(*(u8 **)0x02000710 + 0x5A0) != CROW_AT(u16, 0x2FA))
        SceneActorPlacement_HideActor(arg0, 0);

    SceneActorAnimation_SelectDirectionalEntry(arg0, 0);
    if (CROW_AT(u8, 8) != 0) {
        Anim_Step(CROW_AT(u8 *, 4) + 0x20);
    } else {
        u8 *h = CROW_AT(u8 *, 4);
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

    CROW_AT(u32, 0x304) = 0;
    SceneActorBehavior_InstallCallbackTables(arg0);
    Crow_InitActionTable(arg0);
    Crow_SetBehaviorTable(arg0);
    CROW_AT(u16, 0x2E8) = 2;
    CROW_AT(u16, 0x12E) = CROW_AT(u16, 0x2C0);
    CROW_AT(u16, 0x12C) = CROW_AT(u16, 0x2EA);
    CROW_AT(u32, 0x118) = CROW_AT(u32, 0x2CC);
    CROW_AT(u32, 0x11C) = CROW_AT(u32, 0x2D0);
    CROW_AT(u8, 0x120) = *flagp;
    CROW_AT(u8, 0x121) = CROW_AT(u8, 0x2A8);
    CROW_AT(u8, 0x122) = CROW_AT(u8, 0x2A9);
    CROW_AT(u8, 0x123) = CROW_AT(u8, 0x2AA);
    CROW_AT(u32, 0x124) = CROW_AT(u32, 0x21C);
    CROW_AT(u8, 0x128) = CROW_AT(u8, 0x2B5);
    CROW_AT(void *, 0x24C) = SceneActorScript_OnCommand;
    CROW_AT(void *, 0x258) = Crow_InitPlayerTarget;
    CROW_AT(void *, 0x25C) = Crow_Destroy;

    return SceneActorRegistry_LinkActor(arg0) ? 0 : -1;
}
#else
INCLUDE_ASM("asm/nonmatching", Crow_Init);
#endif
