#include "global.h"
#include "render.h"
#include "engine_globals.h"

extern const u8 gUnk_086108B0[];
extern u8 *gGameState;
extern const u32 gUnk_08000008[];

#define GL_AT(T, off) (*(T *)((u8 *)arg0 + (off)))

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
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void SceneActorControl_OnIdle(void);
void *TerrainModifiers_FindCellOverride(s32, s32);
s32 Div(s32, s32);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
void SceneBody_StartRoute(void *, void *, s32, s32, s32);
void AnimData_SelectPaletteBank(void *);
s32 SceneActorScript_GetManagerField62(void);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorBehavior_InitializeShadow(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
s32 SceneActorRegistry_LinkActor(void *);
s32 Region_FindContaining(void *);
void Path_GetPointPositionById(void *, u32, u32);
s32 Route_GetCurrentPointFacing(void *);
void TargetRegistry_RegisterActor(void *, void *, void *);
void ProximityVolume_TranslateBounds(void *, void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_SinkAndRemove(void);
void SceneActorPlacement_ResetStateThree(void);
void SceneActorReaction_BeginTargetRecovery(void);
void SceneActorReaction_BeginFadedState(void);
void SceneActorReaction_BeginDefeatBurst(void);
void SceneActorReaction_RestoreAfterTurns(void);
void SceneActorReaction_TurnBackToRoute(void);
void SceneActorReaction_UpdateAnimationGate(void);
void SceneActorReaction_ResumeAfterAnimation(void);
void SceneActorReaction_TryResumeRoute(void);
void SceneActorReaction_BeginReactionSound(void);
void Mummy_InitProfile(void *);
void Mummy_InitCollision(void *);
void Mummy_InitActionHandlers(void *);
void Mummy_SetControlPreset(void *);
void Mummy_SetBehaviorTable(void *);
void SceneActorEffects_InitAtActorPosition(void *);
void Mummy_OnUpdate(void);
void Mummy_GetAnimationStepOffset(void);
void Mummy_TurnTowardRoute(void);
void Mummy_UpdateWalking(void);
void Mummy_UpdateIdle(void);
void sub_080D2584(void);
void Mummy_UpdateDefeat(void);
void SceneActorReaction_OnEnemyDefault(void);
void sub_080D7568(void);
void Mummy_UpdateHiddenRecovery(void);
void SceneActorTarget_ResolvePendingObject(void);
void SceneActorReaction_SinkAndRemove(void);
void sub_081363F4(void);
void SceneActorPlacement_ResetStateThree(void);
void Mummy_UpdateDying(void);
void Mummy_ResumeSavedTarget(void);
void SceneActorReaction_BeginTargetRecovery(void);
void Mummy_OnUnusedState(void);
void SceneActorReaction_BeginFadedState(void);
void SceneActorReaction_BeginDefeatBurst(void);
void sub_0807DB34(void);
void SceneActorReaction_RestoreAfterTurns(void);
void sub_0807FCA8(void);
void SceneActorReaction_TurnBackToRoute(void);
void sub_0807F130(void);
void SceneActorReaction_UpdateAnimationGate(void);
void sub_0808170C(void);
void SceneActorReaction_ResumeAfterAnimation(void);
void SceneActorReaction_TryResumeRoute(void);
void SceneActorReaction_BeginReactionSound(void);
void Mummy_ResetRevivalMotion(void);
void SceneActor_EnterMotionPhase(void);
void Mummy_SelectMovementResponse(void);
void Mummy_OnClearActivityFlag(void);
void Mummy_OnMovementUpdate(void);
void sub_080D1644(void);
void sub_080D453C(void);
void SceneActorScript_OnCommand(void);
void Mummy_OnInteractionUpdate(void);
void Mummy_OnReleaseResources(void);
void sub_08080BE0(void);

/* Script_GetPc() ? Script_GetValue() : dflt, inlined at every use */
static inline s32 Mummy_080DCC98_Arg(s32 dflt)
{
    if (Script_GetPc())
        return Script_GetValue();
    return dflt;
}

/* terrain cell index from a packed position pair */
static inline u32 Mummy_080DCC98_CellIndex(s32 x, s32 y)
{
    if (x >= 0 && y >= 0 && (u32)x < gTerrainWidth && (u32)y < gTerrainHeight)
        return *(u16 *)(((u8 *)gTerrainMap) + 0x24 + 2 * y) + x;
    return 0;
}

static inline s32 Mummy_080DCC98_CellHeight(u32 idx, u8 *ref)
{
    void *cell;
    u8 *cp;
    s32 t, h;

    cell = TerrainModifiers_FindCellOverride(idx, 1);
    if (cell == 0)
        cp = (u8 *)(*(u32 *)(((u8 *)gTerrainMap) + 4) + (idx << 2) + 0xC);
    else
        cp = (u8 *)cell + 4;
    t = *cp >> 4;
    h = (*cp & 0xF) << 8;
    if (t == 1)
        h -= ref[4];
    else if (t == 2)
        h -= ref[0];
    return h;
}

#ifdef NONMATCHING
// fuzzy: 33.5%
// scope shape 1546 (differences once low registers are set aside)
// confidence=low: mummy enemy setup: script parameters, route start, animation, callback tables, then LinkActor
s32 Mummy_Init(void *arg0)
{
    u8 *heap;
    u8 *pt;
    u32 zero;
    u32 tmp[2];
    u32 tmp2[2];
    u8 ptbuf[8];
    void *res;
    s32 mode;
    s32 i;
    u32 flag;
    s32 r4v, r5v, r3v, r2v, r1v;
    u8 *r5p;
    u8 *r6p;
    u8 *scr;

    zero = 0;
    GL_AT(u16, 0x6F2) = 0;
    SceneActorRegistry_SetObjectContext(arg0, arg0, 0, 1);
    GL_AT(u8, 0x98) = 1;
    Mummy_InitProfile(arg0);

    if (GL_AT(u8, 8) == 0) {
        heap = Heap_Alloc(0x5C);
        GL_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x5C);
        heap = GL_AT(u8 *, 4);
        AnimData_Load(heap + 0x2C, 0x48DE);
        Sprite_Init(heap, heap + 0x2C, 0);
        SceneActorAnimation_InitEnemySequence(heap, 0xA4B2);
        if (GL_AT(u8, 0x98) == 0x11 || GL_AT(u8, 0x98) == 0x17)
            heap[7] = 1;
        GL_AT(u8, 0x59) = 0;
    } else {
        heap = Heap_Alloc(0x80);
        GL_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x80);
        heap = GL_AT(u8 *, 4);
        CpuSet(&zero, tmp, 0x05000002);
        res = ScriptProc_Start(0xCB05, 0x48DE);
        for (i = 0; i < 8; i++)
            ((u32 *)heap)[i] = ((u32 *)res)[i];
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, tmp);
        GL_AT(u8, 0x2B7) = 0xFF;
    }

    GL_AT(u32, 0x2CC) = 0;
    GL_AT(u32, 0x2D0) = 0;
    GL_AT(u8, 0x2B4) = 0;
    GL_AT(u16, 0x2E8) = 8;
    GL_AT(u16, 0x2C0) = Script_GetKeywordValue(0x4D, 0);
    GL_AT(u16, 0x2C2) = 0x40;
    CpuSet(&zero, tmp2, 0x05000002);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        for (i = 0; i <= 3; i++) {
            GL_AT(u16, 0xB4 + 2 * i) = Mummy_080DCC98_Arg(0x18);
            GL_AT(u16, 0xBC + 2 * i) = Mummy_080DCC98_Arg(0x18);
            GL_AT(u32, 0xCC + 4 * i) = Mummy_080DCC98_Arg(0);
            GL_AT(u16, 0xC4 + 2 * i) = Mummy_080DCC98_Arg(0);
        }
    }

    if (Script_SeekToKeyword(0x6C)) {
        r5v = Mummy_080DCC98_Arg(0x12C);
        r4v = Mummy_080DCC98_Arg(0x12C);
        r2v = Mummy_080DCC98_Arg(0x12C);
    } else {
        r5v = 0;
        r4v = 0;
        r2v = 0;
    }
    GL_AT(u16, 0x414) = 0;
    GL_AT(u16, 0x416) = r2v;
    GL_AT(u16, 0x418) = 0;
    GL_AT(u16, 0x41A) = 0;
    GL_AT(u32, 0x41C) = 0;

    r6p = (u8 *)arg0 + 0x420;
    if (Script_SeekToKeyword(0x6E)) {
        r3v = Mummy_080DCC98_Arg(0);
        *(u16 *)r6p = Mummy_080DCC98_Arg(0);
    } else {
        r3v = 0;
        *(u16 *)r6p = r3v;
    }
    *(u16 *)(r6p + 2) = r3v;
    *(u32 *)(r6p + 4) = *(u16 *)(r6p + 2) * *(u16 *)(r6p + 2);
    r6p[0xA] = 0;

    if (Script_SeekToKeyword(0x65)) {
        r3v = Mummy_080DCC98_Arg(0x30);
        if (Script_GetPc())
            r2v = Script_GetValue() & 0xFF;
        else
            r2v = 0x40;
        r1v = r3v;
    } else {
        r3v = 0x30;
        r1v = 0x30;
        r2v = 0x40;
    }
    GL_AT(u32, 0xDC) = 0;
    GL_AT(u16, 0xEC) = r3v;
    GL_AT(u16, 0xEE) = r1v;
    GL_AT(u32, 0xF8) = r3v * r3v;
    GL_AT(u32, 0xFC) = r1v * r1v;
    GL_AT(u8, 0xEA) = r2v;
    if (GL_AT(u8, 0xEB) & 4)
        GL_AT(u32, 0xF4) = GL_AT(u32, 0xF8);
    else
        GL_AT(u32, 0xF4) = Div(GL_AT(u32, 0xF8) * 8 - GL_AT(u32, 0xF8), 10);

    if (Script_GetKeywordValue(0x62, 0))
        GL_AT(u32, 0x2D0) |= 0x200;
    GL_AT(u32, 0x1F0) = (u16)Script_GetKeywordValue(0x46, 0);
    GL_AT(u32, 0x1EC) = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        r1v = (u16)Mummy_080DCC98_Arg(0);
        GL_AT(u16, 0xB0) = r1v;
        if (r1v < 0)
            r1v = r1v >> 1;
        else
            r1v = -((-r1v) >> 1);
        GL_AT(u16, 0x5B6) = r1v;
        GL_AT(u8, 0x9E) = Mummy_080DCC98_Arg(0);
        GL_AT(u8, 0x9F) = Mummy_080DCC98_Arg(0);
        GL_AT(u8, 0xA0) = Mummy_080DCC98_Arg(0);
        GL_AT(u16, 0xA2) = Mummy_080DCC98_Arg(0);
        GL_AT(u16, 0xA4) = Mummy_080DCC98_Arg(0);
    }

    r1v = GL_AT(u16, 0xB0);
    r3v = r4v;
    GL_AT(u16, 0x2D4) = 0;
    GL_AT(void *, 0x21C) = SceneActorControl_OnIdle;
    GL_AT(u8, 0x2B2) = 1;
    GL_AT(u8, 0x2B0) = 0;
    if ((s16)r1v <= 0)
        r1v = 0;
    GL_AT(u16, 0x9C) = r1v;
    GL_AT(u16, 0xAA) = 0;
    if (r3v <= 0)
        r3v = r5v;
    GL_AT(u16, 0xA6) = r5v;
    GL_AT(u16, 0xA8) = r3v;
    GL_AT(u16, 0x2FA) = 0;

    if (Script_SeekToKeyword(0x58)) {
        GL_AT(u32, 0xAC) = Mummy_080DCC98_Arg(0);
        GL_AT(u8, 0xA1) = Mummy_080DCC98_Arg(0);
    }
    GL_AT(u16, 0xB2) = Script_GetKeywordValue(0x4B, 2);
    r1v = Script_GetKeywordValue(0x56, -1);
    if ((s16)r1v < 0)
        r1v = 0xFFFF;
    GL_AT(u16, 0x9A) = r1v;
    if (Script_GetKeywordValue(0x48, 0))
        GL_AT(u32, 0x2D0) |= 0x800;
    if (Script_GetKeywordValue(0x4C, 0))
        GL_AT(u32, 0x2D0) |= 0x8000;

    SceneBody_Init((u8 *)arg0 + 0x50, mode, tmp2, 0, 4, arg0);
    r5p = (u8 *)arg0 + 0x448;
    r5p[0xF] = 0;

    if (Script_SeekToKeyword(0x72)) {
        r1v = Mummy_080DCC98_Arg(0);
        r5p[0x10] = r1v;
        r5p[0x11] = 0;
        r2v = Mummy_080DCC98_Arg(0);
        r5p[0x12] = r2v;
        r5p[0xC] = r2v;
        r5p[0xD + r5p[0xF]] = r5p[0x10];
        if (Script_GetPc())
            r5p[0xD + (r5p[0xF] ^ 1)] = Script_GetValue();
        else
            r5p[0xD + (r5p[0xF] ^ 1)] = r5p[0x10];
        pt = ptbuf;
        Path_GetPointPositionById(pt, r5p[0x10], r5p[0x12]);
        GL_AT(u16, 0x2F4) =
            Mummy_080DCC98_CellIndex((s8)(*(u16 *)pt >> 8), (s8)(*(u16 *)(pt + 4) >> 8));
        r1v = Mummy_080DCC98_CellHeight(GL_AT(u16, 0x2F4), pt);
        GL_AT(u16, 0x2BE) = r1v;
        *(u16 *)(pt + 2) = r1v;
        SceneBody_StartRoute((u8 *)arg0 + 0x50, r5p, r5p[0x10], r5p[0x11], r5p[0x12]);
        r6p = (u8 *)arg0 + 0x5C;
        *(u16 *)r6p = **(u16 **)(r5p + 8);
        *(u16 *)(r6p + 4) = (*(u16 **)(r5p + 8))[1];
        r1v = Mummy_080DCC98_CellHeight(
            Mummy_080DCC98_CellIndex((s8)(*(u16 *)r6p >> 8), (s8)(*(u16 *)(r6p + 4) >> 8)), r6p);
        *(u16 *)(r6p + 2) = r1v;
        if (GL_AT(u8, 8) == 0) {
            u8 *h = GL_AT(u8 *, 4);
            *(u32 *)(h + 0x1C) = GL_AT(u32, 0x5C);
            *(u32 *)(h + 0x20) = GL_AT(u32, 0x60);
        } else {
            u8 *h = GL_AT(u8 *, 4);
            *(u32 *)(h + 0x40) = GL_AT(u32, 0x5C);
            *(u32 *)(h + 0x44) = GL_AT(u32, 0x60);
        }
        GL_AT(u8, 0x59) = Route_GetCurrentPointFacing(r5p);
    } else {
        r5p[0x10] = 0;
        r5p[0x11] = 0;
        r5p[0x12] = 0;
        r5p[0xC] = 0;
        r5p[0xD + r5p[0xF]] = 0;
        r5p[0xD + (r5p[0xF] ^ 1)] = 0;
        r6p = (u8 *)arg0 + 0x5C;
    }

    if (GL_AT(u16, 0x2C0) & 0x200)
        TargetRegistry_RegisterActor((u8 *)arg0 + 0x404, (u8 *)arg0 + 0x50, sub_08080BE0);
    SceneBody_SetSprite((u8 *)arg0 + 0x50, GL_AT(u8, 8) ? 0 : GL_AT(u8 *, 4));
    if (GL_AT(u8, 0x98) == 0x15)
        SceneBody_SetMovementProbe((u8 *)arg0 + 0x50, (u8 *)arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe((u8 *)arg0 + 0x50, (u8 *)arg0 + 0x40, 0x40, 0x40);
    GL_AT(u16, 0x3D4) = GL_AT(u32, 0x50);
    GL_AT(u32, 0x3D8) = (u32)r6p;

    GL_AT(u16, 0x1E6) = Script_GetKeywordValue(0x43, 0);
    GL_AT(u16, 0x1E0) = 0x23;
    GL_AT(u16, 0x1E2) = 0x192;
    GL_AT(u16, 0x1E4) = GL_AT(u16, 0x1E6) + 0x23;
    r1v = GL_AT(u16, 0x1E0) + GL_AT(u16, 0x1E6);
    if (GL_AT(u8, 8) == 0)
        AnimData_SelectPaletteBank(GL_AT(u8 *, 4) + 0x2C);
    else
        *(u8 **)(GL_AT(u8 *, 4) + 0x68) = gPaletteBankData + (r1v << 5);
    if (GL_AT(u8, 8) == 1) {
        r1v = SceneActorScript_GetManagerField62();
        *(u16 *)(GL_AT(u8 *, 4) + 0x20 + 0x3A) = r1v - 0x5FFF;
    }
    GL_AT(u16, 0x1E4) = GL_AT(u16, 0x1E0) + 3;
    GL_AT(u16, 0x1DC) = GL_AT(u16, 0x1E0) + 4;
    GL_AT(u16, 0x1DE) = GL_AT(u16, 0x1E0);

    Mummy_InitCollision(arg0);
    GL_AT(u32, 0x2CC) |= 0x2000;
    GL_AT(void *, 0x274) = Mummy_OnUpdate;
    GL_AT(void *, 0x260) = Mummy_GetAnimationStepOffset;
    GL_AT(void *, 0x224) = Mummy_TurnTowardRoute;
    GL_AT(void *, 0x228) = Mummy_UpdateWalking;
    GL_AT(void *, 0x22C) = Mummy_UpdateIdle;
    GL_AT(void *, 0x230) = sub_080D2584;
    GL_AT(void *, 0x234) = Mummy_UpdateDefeat;
    GL_AT(void *, 0x220) = Mummy_UpdateHiddenRecovery;
    GL_AT(void *, 0x278) = SceneActorReaction_OnEnemyDefault;
    GL_AT(void *, 0x270) = sub_080D7568;
    GL_AT(const u8 *, 0x250) = gUnk_086108B0;
    GL_AT(u32, 0x248) = 0;
    GL_AT(void *, 0x26C) = SceneActorTarget_ResolvePendingObject;
    GL_AT(void *, 0x238) = SceneActorReaction_SinkAndRemove;
    GL_AT(void *, 0x264) = sub_081363F4;
    GL_AT(void *, 0x268) = SceneActorPlacement_ResetStateThree;
    GL_AT(u32, 0x240) = 0;
    GL_AT(u32, 0x104) = 0;
    GL_AT(u32, 0x108) = 0;
    GL_AT(u32, 0x10C) = 0;
    GL_AT(u32, 0x110) = 0;
    GL_AT(u32, 0x114) = 0;
    GL_AT(void *, 0x5D0) = (u8 *)arg0 + 0x6F4;
    GL_AT(void *, 0x5D4) = (u8 *)arg0 + 0x718;
    GL_AT(void *, 0x5F4) = Mummy_UpdateDying;
    GL_AT(void *, 0x5F8) = SceneActorReaction_BeginTargetRecovery;
    GL_AT(void *, 0x5FC) = SceneActorReaction_BeginFadedState;
    GL_AT(void *, 0x600) = SceneActorReaction_BeginDefeatBurst;
    GL_AT(void *, 0x43C) = Mummy_ResumeSavedTarget;
    GL_AT(u32, 0x440) = 0;
    GL_AT(u8, 0x42B) = 3;
    GL_AT(void *, 0x618) = sub_0807DB34;
    GL_AT(u32, 0x608) = 0;
    GL_AT(u32, 0x61C) = 0;
    GL_AT(u32, 0x620) = 0;
    GL_AT(void *, 0x5F0) = SceneActorReaction_RestoreAfterTurns;
    GL_AT(void *, 0x60C) = sub_0807FCA8;
    GL_AT(void *, 0x604) = Mummy_OnUnusedState;
    GL_AT(void *, 0x614) = SceneActorReaction_TurnBackToRoute;
    GL_AT(u32, 0x5E8) = 0;
    GL_AT(void *, 0x444) = sub_0807F130;
    GL_AT(void *, 0x41C) = SceneActorReaction_UpdateAnimationGate;
    GL_AT(u16, 0x41A) = 0x28;
    GL_AT(void *, 0x5DC) = sub_0808170C;
    GL_AT(void *, 0x5E0) = SceneActorReaction_ResumeAfterAnimation;
    GL_AT(void *, 0x5E4) = SceneActorReaction_TryResumeRoute;
    GL_AT(void *, 0x610) = SceneActorReaction_BeginReactionSound;
    ProximityVolume_TranslateBounds((u8 *)arg0 + 0x6F4, (u8 *)arg0 + 0x5C);
    SceneActorMovement_ResolveRoutePosition(arg0);
    GL_AT(void *, 0x248) = Mummy_ResetRevivalMotion;
    GL_AT(void *, 0x23C) = SceneActor_EnterMotionPhase;
    GL_AT(void *, 0x268) = Mummy_SelectMovementResponse;
    GL_AT(void *, 0x61C) = Mummy_OnClearActivityFlag;
    GL_AT(void *, 0x5EC) = Mummy_OnMovementUpdate;
    GL_AT(void *, 0x5E8) = sub_080D1644;
    GL_AT(void *, 0x444) = sub_080D453C;
    GL_AT(u16, 0x2E8) = 1;
    GL_AT(u8, 0xDC + 0xF) |= 1;
    GL_AT(u32, 0x2CC) |= 0x600000;
    GL_AT(u32, 0x2D0) |= 0x8000000;
    GL_AT(u16, 0x13A) &= 0xFFFE;
    SceneActorBehavior_InitializeShadow(arg0);
    SceneActorEffects_InitAtActorPosition(arg0);
    SceneActorTerrain_ReadScriptParameters(arg0);

    r2v = Script_GetKeywordValue(0x75, 0);
    GL_AT(u16, 0x5CC) = 0x21;
    if (r2v != 0) {
        GL_AT(u32, 0x2CC) |= 0x40000;
        if (r2v == 2)
            GL_AT(u32, 0x2CC) |= 0x80000;
    }

    flag = (GL_AT(u16, 0x2C0) & 4) != 0;
    if (flag)
        GL_AT(u32, 0x2D0) |= 0x2000;

    if (!flag) {
        GL_AT(u8, 0x2A8) = 0;
        GL_AT(u8, 0x2A9) = 0;
        GL_AT(u8, 0x2AA) = 0;
        GL_AT(u32, 0x2AC) = 0;
        GL_AT(u8, 0x2B1) = 1;
        GL_AT(u8, 0x2B2) = 1;
        GL_AT(u8, 0x2B0) = 0;
        GL_AT(u8, 0x2B5) = 0;
        GL_AT(u32, 0x21C) = GL_AT(u32, 0x22C);
        GL_AT(u16, 0x2C8) = 0;
        GL_AT(u32, 0x114) &= ~1;
        GL_AT(u32, 0x104) = 0;
    }

    if (GL_AT(u16, 0x2C0) & 2)
        GL_AT(u16, 0x2FA) = *(u32 *)(gGameState + 0x5A0);
    else
        GL_AT(u16, 0x2FA) = Region_FindContaining((u8 *)arg0 + 0x5C);

    if (GL_AT(u16, 0x2C0) & 0x40)
        GL_AT(u16, 0x13A) &= 0xFEFF;

    if (Script_GetKeywordValue(0x53, 0) != 0 || *(u32 *)(gGameState + 0x5A0) != GL_AT(u16, 0x2FA))
        SceneActorPlacement_HideActor(arg0, 0);

    SceneActorAnimation_SelectDirectionalEntry(arg0, 0);
    if (GL_AT(u8, 8) != 0) {
        Anim_Step(GL_AT(u8 *, 4) + 0x20);
    } else {
        u8 *h = GL_AT(u8 *, 4);
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

    GL_AT(u32, 0x304) = 0;
    SceneActorBehavior_InstallCallbackTables(arg0);
    Mummy_SetControlPreset(arg0);
    Mummy_InitActionHandlers(arg0);
    Mummy_SetBehaviorTable(arg0);
    GL_AT(u16, 0x6E8) = GL_AT(u16, 0xEC);
    scr = GL_AT(u8 *, 0x3D0);
    if (Script_SeekToKeyword(0x42)) {
        *(u16 *)(scr + 0x6CC) = Mummy_080DCC98_Arg(0x14);
        *(u16 *)(scr + 0x6CE) = Mummy_080DCC98_Arg(0xB4);
    } else {
        *(u16 *)(scr + 0x6CC) = 0x14;
        *(u16 *)(scr + 0x6CE) = 0xB4;
    }
    scr = GL_AT(u8 *, 0x3D0);
    *(u32 *)(scr + 0x6D8) = Div(GL_AT(u32, 0xF4) * 3, 10);
    *(u32 *)(scr + 0x6DC) = GL_AT(u32, 0xF4);
    GL_AT(u16, 0x118 + 0x16) = GL_AT(u16, 0x2C0);
    GL_AT(u16, 0x118 + 0x14) = GL_AT(u16, 0x2EA);
    GL_AT(u32, 0x118) = GL_AT(u32, 0x2CC);
    GL_AT(u32, 0x118 + 4) = GL_AT(u32, 0x2D0);
    GL_AT(u8, 0x118 + 8) = GL_AT(u8, 0x59);
    GL_AT(u8, 0x118 + 9) = GL_AT(u8, 0x2A8);
    GL_AT(u8, 0x118 + 0xA) = GL_AT(u8, 0x2A9);
    GL_AT(u8, 0x118 + 0xB) = GL_AT(u8, 0x2AA);
    GL_AT(u32, 0x118 + 0xC) = GL_AT(u32, 0x21C);
    GL_AT(u8, 0x118 + 0x10) = GL_AT(u8, 0x2B5);
    GL_AT(void *, 0x24C) = SceneActorScript_OnCommand;
    GL_AT(void *, 0x258) = Mummy_OnInteractionUpdate;
    GL_AT(void *, 0x25C) = Mummy_OnReleaseResources;

    return SceneActorRegistry_LinkActor(arg0) ? 0 : -1;
}
#else
INCLUDE_ASM("asm/nonmatching", Mummy_Init);
#endif
