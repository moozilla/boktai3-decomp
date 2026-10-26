#include "global.h"
#include "render.h"

/* Script_GetPc() ? Script_GetValue() : dflt, inlined at every use */

// confidence=low: bat enemy setup: script parameters, animation tables, callback tables

void SceneActorControl_UpdateFacingAnim(void);

void SceneActorControl_CompleteScripted(void);

void SceneActorControl_TryActivateProximity(void);

/* Script_GetPc() ? Script_GetValue() : dflt, inlined at every use */

// confidence=low: bat enemy setup: script parameters, animation tables, callback tables

/* Script_GetPc() ? Script_GetValue() : dflt, inlined at every use */

// confidence=low: bat enemy setup: script parameters, animation tables, callback tables
extern const u8 gUnk_08605C74[];

#define BAT_AT(T, off) (*(T *)((u8 *)arg0 + (off)))

s32 Script_SeekToKeyword(u32);
s32 Script_GetKeywordValue(u32, s32);
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
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
void SceneActorControl_SetDefaultProfile(void *);
void SceneActorControl_OnIdle(void);
void Bat_OnUpdate(void);
void SceneActorControl_GetTravelProfile(void);
void SceneActorControl_UpdateFacingAnimation(void);
void SceneActorControl_ResetDirectionalAnimation(void);
void SceneActorControl_ConvergeSavedMotion(void);
void SceneActorControl_CompleteScriptedResponse(void);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolveDamage(void);
void SceneActorPlacement_PlayResponseAndResetState(void);
void SceneActorReaction_SinkAndRemove(void);
void SceneActorControl_RestoreMotion(void);
void SceneActorControl_SetCollisionResponse(void);
void SceneActorControl_TryActivateProximityResponse(void);
void SceneActorScript_OnCommand(void);
void SceneActorControl_BindPrimaryPlayer(void);
void Bat_Destroy(void);
void *TerrainModifiers_FindCellOverride(s32, s32);
s32 Div(s32, s32);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
void AnimData_SelectPaletteBank(void *);
s32 SceneActorScript_GetManagerField62(void);
void SceneActorControl_InitContactColliders(void *);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *);
void SceneActorRegistry_RegisterCategoryReceiver(void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorControl_SetDefaultPreset(void *);
void SceneActorControl_SetApproachCallbacks(void *);
void SceneActorControl_SetStoredMotionCallbacks(void *);
void SceneActorControl_RestartPositionCurve(void *);
void ActorShadow_Disable(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
s32 SceneActorRegistry_LinkActor(void *);
s32 Region_FindContaining(void *);

/* Script_GetPc() ? Script_GetValue() : dflt, inlined at every use */
static inline s32 Bat_08084F44_Arg(s32 dflt)
{
    if (Script_GetPc())
        return Script_GetValue();
    return dflt;
}

#ifdef NONMATCHING
// fuzzy: 24.1%
// scope shape 1235 (differences once low registers are set aside)
// confidence=low: bat enemy setup: script parameters, animation tables, callback tables
s32 Bat_Init(void *arg0)
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
    BAT_AT(u32, 0x3D4) = 0;
    SceneActorRegistry_SetObjectContext(arg0, arg0, 0);
    BAT_AT(u8, 0x98) = 0x12;
    SceneActorControl_SetDefaultProfile(arg0);

    if (BAT_AT(u8, 8) == 0) {
        heap = Heap_Alloc(0x5C);
        BAT_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x5C);
        heap = BAT_AT(u8 *, 4);
        AnimData_Load(heap + 0x2C, 0xDA6D);
        Sprite_Init(heap, heap + 0x2C, 0);
        SceneActorAnimation_InitEnemySequence(heap, 0x5BB7);
        if (BAT_AT(u8, 0x98) == 0x11 || BAT_AT(u8, 0x98) == 0x17)
            heap[7] = 1;
        BAT_AT(u8, 0x59) = 0;
        flagp = (u8 *)arg0 + 0x59;
    } else {
        heap = Heap_Alloc(0x80);
        BAT_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x80);
        heap = BAT_AT(u8 *, 4);
        CpuSet(&zero, tmp, 0x05000002);
        res = ScriptProc_Start(0xCB05, 0xDA6D);
        for (i = 0; i < 8; i++)
            ((u32 *)heap)[i] = ((u32 *)res)[i];
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, tmp);
        BAT_AT(u8, 0x2B7) = 0xFF;
        flagp = (u8 *)arg0 + 0x59;
    }

    BAT_AT(u32, 0x2CC) = 0;
    BAT_AT(u32, 0x2D0) = 0;
    BAT_AT(u8, 0x2B4) = 0;
    BAT_AT(u16, 0x2E8) = 8;
    BAT_AT(u16, 0x2C0) = Script_GetKeywordValue(0x4D, 0);
    CpuSet(&zero, tmp2, 0x05000002);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        for (i = 0; i <= 3; i++) {
            BAT_AT(u16, 0xB4 + 2 * i) = Bat_08084F44_Arg(0x18);
            BAT_AT(u16, 0xBC + 2 * i) = Bat_08084F44_Arg(0x18);
            BAT_AT(u32, 0xCC + 4 * i) = Bat_08084F44_Arg(0);
            BAT_AT(u16, 0xC4 + 2 * i) = Bat_08084F44_Arg(0);
        }
    }

    if (Script_SeekToKeyword(0x65)) {
        r4v = Bat_08084F44_Arg(0x30);
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
    BAT_AT(u32, 0xDC) = 0;
    BAT_AT(u16, 0xEC) = r4v;
    BAT_AT(u16, 0xEE) = r1v;
    BAT_AT(u8, 0xEA) = r2v;
    BAT_AT(u32, 0xF8) = r4v * r4v;
    BAT_AT(u32, 0xFC) = r1v * r1v;
    if (BAT_AT(u8, 0xEB) & 4)
        BAT_AT(u32, 0xF4) = BAT_AT(u32, 0xF8);
    else
        BAT_AT(u32, 0xF4) = Div(BAT_AT(u32, 0xF8) * 8 - BAT_AT(u32, 0xF8), 10);

    if (Script_SeekToKeyword(0x6C)) {
        r6v = Bat_08084F44_Arg(0x12C);
        r4v = Bat_08084F44_Arg(0x12C);
    } else {
        r6v = 0;
        r4v = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        BAT_AT(u32, 0x2D0) |= 0x200;
    BAT_AT(u32, 0x1F0) = (u16)Script_GetKeywordValue(0x46, 0);
    BAT_AT(u32, 0x1EC) = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        BAT_AT(u16, 0xB0) = Bat_08084F44_Arg(0);
        BAT_AT(u8, 0x9E) = Bat_08084F44_Arg(0);
        BAT_AT(u8, 0x9F) = Bat_08084F44_Arg(0);
        BAT_AT(u8, 0xA0) = Bat_08084F44_Arg(0);
        BAT_AT(u16, 0xA2) = Bat_08084F44_Arg(0);
        BAT_AT(u16, 0xA4) = Bat_08084F44_Arg(0);
    }

    r1v = BAT_AT(u16, 0xB0);
    r3v = r4v;
    BAT_AT(u16, 0x2D4) = 0;
    BAT_AT(void *, 0x21C) = SceneActorControl_OnIdle;
    BAT_AT(u8, 0x2B2) = 1;
    BAT_AT(u8, 0x2B0) = 0;
    if ((s16)r1v <= 0)
        r1v = 0;
    BAT_AT(u16, 0x9C) = r1v;
    BAT_AT(u16, 0xAA) = 0;
    if (r3v <= 0)
        r3v = r6v;
    BAT_AT(u16, 0xA6) = r6v;
    BAT_AT(u16, 0xA8) = r3v;
    BAT_AT(u16, 0x2FA) = 0;

    if (Script_SeekToKeyword(0x58)) {
        BAT_AT(u32, 0xAC) = Bat_08084F44_Arg(0);
        BAT_AT(u8, 0xA1) = Bat_08084F44_Arg(0);
    }
    BAT_AT(u16, 0xB2) = Script_GetKeywordValue(0x4B, 2);
    r1v = Script_GetKeywordValue(0x56, -1);
    if ((s16)r1v < 0)
        r1v = 0xFFFF;
    BAT_AT(u16, 0x9A) = r1v;
    if (Script_GetKeywordValue(0x48, 0))
        BAT_AT(u32, 0x2D0) |= 0x800;

    SceneBody_Init((u8 *)arg0 + 0x50, mode, tmp2, 0, 4, arg0);
    SceneBody_SetSprite((u8 *)arg0 + 0x50, BAT_AT(u8, 8) ? 0 : BAT_AT(u8 *, 4));
    SceneBody_SetMovementProbe((u8 *)arg0 + 0x50, (u8 *)arg0 + 0x40, 0x40, 0x40);

    if (Script_SeekToKeyword(0x67)) {
        BAT_AT(u8, 0x3DA) = Bat_08084F44_Arg(0);
        BAT_AT(u8, 0x3DC) = Bat_08084F44_Arg(0);
    } else {
        BAT_AT(u8, 0x3DA) = 0x3C;
        BAT_AT(u8, 0x3DC) = 3;
    }

    if (Script_GetKeywordValue(0x45, 0) == 0)
        *flagp = 0;
    else
        *flagp = 0x40;

    if (Script_SeekToKeyword(0x70)) {
        BAT_AT(u16, 0x5C) = Bat_08084F44_Arg(0);
        BAT_AT(u16, 0x5E) = Bat_08084F44_Arg(0);
        BAT_AT(u16, 0x60) = Bat_08084F44_Arg(0);
        w0 = BAT_AT(u32, 0x5C);
        w1 = BAT_AT(u32, 0x60);
        if (*flagp == 0)
            w0 = (w0 & 0xFFFF0000) | (u16)((u16)w0 + 0x80);
        else
            w1 = (w1 & 0xFFFF0000) | (u16)((u16)w1 + 0x80);
        BAT_AT(u32, 0x434) = w0;
        BAT_AT(u32, 0x438) = w1;
        x = (s8)((u16)w0 >> 8);
        y = (s8)((u16)w1 >> 8);
        if (x >= 0 && y >= 0 && (u32)x < *(u32 *)0x030052F8 && (u32)y < *(u32 *)0x030052FC)
            r1v = *(u16 *)(*(u8 **)0x030052F4 + 0x24 + 2 * y) + x;
        else
            r1v = 0;
        BAT_AT(u16, 0x2F4) = r1v;
        r1v = BAT_AT(u16, 0x2F4);
        cell = TerrainModifiers_FindCellOverride(r1v, 1);
        if (cell == 0)
            cellp = (u8 *)(*(u32 *)(*(u8 **)0x030052F4 + 4) + (r1v << 2) + 0xC);
        else
            cellp = (u8 *)cell + 4;
        r2v = *cellp >> 4;
        r1v = (*cellp & 0xF) << 8;
        if (r2v == 1)
            r1v -= BAT_AT(u8, 0x434 + 4);
        else if (r2v == 2)
            r1v -= BAT_AT(u8, 0x434);
        BAT_AT(u16, 0x2BE) = r1v;
        BAT_AT(u16, 0x2EA) = BAT_AT(u16, 0x5E) - r1v;
        BAT_AT(u16, 0x3D8) = BAT_AT(u16, 0x2EA);
        BAT_AT(u16, 0x5E) = BAT_AT(u16, 0x2BE);
        BAT_AT(u16, 0x3DE) = BAT_AT(u16, 0x2BE);
        BAT_AT(u16, 0x3EA) = BAT_AT(u16, 0x2BE);
    }

    BAT_AT(u16, 0x1E6) = Script_GetKeywordValue(0x43, 0);
    BAT_AT(u16, 0x1E0) = 0x2D2;
    BAT_AT(u16, 0x1E2) = 0x192;
    BAT_AT(u16, 0x1E4) = BAT_AT(u16, 0x1E6) + 0x2D4;
    r1v = BAT_AT(u16, 0x1E0) + BAT_AT(u16, 0x1E6);
    if (BAT_AT(u8, 8) == 0)
        AnimData_SelectPaletteBank(BAT_AT(u8 *, 4) + 0x2C);
    else
        *(u8 **)(BAT_AT(u8 *, 4) + 0x68) = gPaletteBankData + (r1v << 5);
    if (BAT_AT(u8, 8) == 1) {
        r1v = SceneActorScript_GetManagerField62();
        *(u16 *)(BAT_AT(u8 *, 4) + 0x20 + 0x3A) = r1v - 0x5FFF;
    }

    SceneActorControl_InitContactColliders(arg0);
    BAT_AT(void *, 0x274) = Bat_OnUpdate;
    BAT_AT(void *, 0x260) = SceneActorControl_GetTravelProfile;
    BAT_AT(u32, 0x224) = 0;
    BAT_AT(void *, 0x228) = SceneActorControl_UpdateFacingAnimation;
    BAT_AT(void *, 0x22C) = SceneActorControl_ResetDirectionalAnimation;
    BAT_AT(void *, 0x230) = SceneActorControl_ConvergeSavedMotion;
    BAT_AT(void *, 0x234) = SceneActorControl_CompleteScriptedResponse;
    BAT_AT(u32, 0x220) = 0;
    BAT_AT(void *, 0x278) = SceneActorReaction_OnEnemyDefault;
    BAT_AT(u32, 0x270) = 0;
    BAT_AT(u32, 0x248) = 0;
    BAT_AT(u32, 0x26C) = 0;
    BAT_AT(void *, 0x238) = SceneActorReaction_SinkAndRemove;
    BAT_AT(void *, 0x264) = SceneActorReaction_ResolveDamage;
    BAT_AT(void *, 0x268) = SceneActorPlacement_PlayResponseAndResetState;
    BAT_AT(u32, 0x240) = 0;
    BAT_AT(u32, 0x104) = 0;
    BAT_AT(u32, 0x108) = 0;
    BAT_AT(u32, 0x10C) = 0;
    BAT_AT(u32, 0x110) = 0;
    BAT_AT(u32, 0x114) = 0;
    BAT_AT(const u8 *, 0x250) = gUnk_08605C74;
    BAT_AT(void *, 0x248) = SceneActorControl_RestoreMotion;
    BAT_AT(void *, 0x23C) = SceneActorControl_SetCollisionResponse;
    BAT_AT(void *, 0x270) = SceneActorControl_TryActivateProximityResponse;
    BAT_AT(u32, 0x2D0) |= 0x02100408;
    BAT_AT(u16, 0x2C0) |= 0x60;
    SceneActorBehavior_InitializeShadow(arg0);
    SceneActorTerrain_ReadScriptParameters(arg0);
    flag = (BAT_AT(u16, 0x2C0) & 4) != 0;
    if (flag)
        BAT_AT(u32, 0x2D0) |= 0x2000;

    if (!flag) {
        BAT_AT(u8, 0x2A8) = 0;
        BAT_AT(u8, 0x2A9) = 0;
        BAT_AT(u8, 0x2AA) = 0;
        BAT_AT(u32, 0x2AC) = 0;
        BAT_AT(u8, 0x2B1) = 1;
        BAT_AT(u8, 0x2B2) = 1;
        BAT_AT(u8, 0x2B0) = 0;
        BAT_AT(u8, 0x2B5) = 0;
        BAT_AT(u32, 0x21C) = BAT_AT(u32, 0x22C);
        BAT_AT(u16, 0x2C8) = 0;
        BAT_AT(u32, 0x114) &= ~1;
        BAT_AT(u32, 0x104) = 0;
    }

    if (BAT_AT(u16, 0x2C0) & 2)
        BAT_AT(u16, 0x2FA) = *(u32 *)(*(u8 **)0x02000710 + 0x5A0);
    else
        BAT_AT(u16, 0x2FA) = Region_FindContaining((u8 *)arg0 + 0x5C);

    if (BAT_AT(u16, 0x2C0) & 0x40)
        BAT_AT(u16, 0x13A) &= 0xFEFF;

    if (Script_GetKeywordValue(0x53, 0) != 0 ||
        *(u32 *)(*(u8 **)0x02000710 + 0x5A0) != BAT_AT(u16, 0x2FA))
        SceneActorPlacement_HideActor(arg0, 0);

    SceneActorAnimation_SelectDirectionalEntry(arg0, 0);
    if (BAT_AT(u8, 8) != 0) {
        Anim_Step(BAT_AT(u8 *, 4) + 0x20);
    } else {
        u8 *h = BAT_AT(u8 *, 4);
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

    BAT_AT(u32, 0x304) = 0;
    SceneActorRegistry_RegisterCategoryReceiver(arg0);
    SceneActorBehavior_InstallCallbackTables(arg0);
    SceneActorControl_SetDefaultPreset(arg0);
    SceneActorControl_SetApproachCallbacks(arg0);
    SceneActorControl_SetStoredMotionCallbacks(arg0);
    BAT_AT(u16, 0x2E8) = 2;
    BAT_AT(u8 *, 4)[7] = 1;
    if (Script_GetKeywordValue(0x54, 0) == 0) {
        SceneActorControl_RestartPositionCurve(arg0);
    } else {
        BAT_AT(u32, 0x3D4) |= 2;
        ActorShadow_Disable((u8 *)arg0 + 0x320);
    }

    BAT_AT(u16, 0x118 + 0x16) = BAT_AT(u16, 0x2C0);
    BAT_AT(u16, 0x118 + 0x14) = BAT_AT(u16, 0x2EA);
    BAT_AT(u32, 0x118) = BAT_AT(u32, 0x2CC);
    BAT_AT(u32, 0x118 + 4) = BAT_AT(u32, 0x2D0);
    BAT_AT(u8, 0x118 + 8) = *flagp;
    BAT_AT(u8, 0x118 + 9) = BAT_AT(u8, 0x2A8);
    BAT_AT(u8, 0x118 + 0xA) = BAT_AT(u8, 0x2A9);
    BAT_AT(u8, 0x118 + 0xB) = BAT_AT(u8, 0x2AA);
    BAT_AT(u32, 0x118 + 0xC) = BAT_AT(u32, 0x21C);
    BAT_AT(u8, 0x118 + 0x10) = BAT_AT(u8, 0x2B5);
    BAT_AT(void *, 0x24C) = SceneActorScript_OnCommand;
    BAT_AT(void *, 0x258) = SceneActorControl_BindPrimaryPlayer;
    BAT_AT(void *, 0x25C) = Bat_Destroy;

    return SceneActorRegistry_LinkActor(arg0) ? 0 : -1;
}
#else
INCLUDE_ASM("asm/nonmatching", Bat_Init);
#endif
