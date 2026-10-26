#include "global.h"
#include "render.h"

#define SP_AT(T, off) (*(T *)((u8 *)arg0 + (off)))
#define SV(d)         (Script_GetPc() ? Script_GetValue() : (d))

struct Sp32Block {
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
void Spider_InitProfile(void *);
void Spider_OnUpdate(void);
void Spider_GetAnimationEvent(void);
void sub_080A21EC(void);
void Spider_SelectActiveAnimation(void);
void Spider_SelectIdleAnimation(void);
void Spider_UpdateLandingMotion(void);
void Spider_UpdateDefeat(void);
void Spider_OnActionEvent(void);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorPlacement_BeginContactRoute(void);
void SceneActorReaction_SinkAndRemove(void);
void sub_081363F4(void);
void SceneActorPlacement_BeginDefaultResponse(void);
void Spider_OnSecondaryColliderContact(void);
void SceneActorReaction_BeginTargetRecovery(void);
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
void SceneActorRoute_ResetStep(void);
void SceneActorReaction_ResumeEntryTarget(void);
void SceneActorPlacement_BeginNearbyPlayerContact(void);
void SceneActorPlacement_SelectPlayerApproach(void);
void Spider_UpdateHiddenRecovery(void);
void SceneActorReaction_RestoreAlternateAfterDelay(void);
void Spider_OnSecondaryUpdate(void);
void Spider_Destroy(void);
void SceneActorScript_OnCommand(void);
void SceneActorControl_OnIdle(void);
void sub_08080BE0(void);
void Spider_InitColliders(void *);
void Spider_SetBehaviorTables(void *);
void ProximityVolume_TranslateBounds(void *, void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern const u8 gUnk_08605F34[];
extern const u8 gUnk_086108B0[];
void *TerrainModifiers_FindCellOverride(s32, s32);
s32 Div(s32, s32);
void SceneBody_Init(void *, s32, void *, s32, s32, void *);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
s32 Path_GetPointPositionById(u16 *, u32, u32);
u32 Route_GetCurrentPointFacing(void *);
s32 TargetRegistry_RegisterActor(void *, void *, void *);
void SceneBody_SetSprite(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, s32, s32);
void AnimData_SelectPaletteBank(void *);
s32 SceneActorScript_GetManagerField62(void);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
s32 SceneActorRegistry_LinkActor(void *);
void *Region_FindContaining(void *);

extern u32 *gUnk_030052F4;
extern u32 gUnk_030052F8;
extern u32 gUnk_030052FC;
extern u8 *gUnk_02000710;

/* terrain height probe: (x, y) world pos -> cell id -> height offset */
static inline s32 Spider_CellHeight(u16 *pos, u16 *cellOut)
{
    s32 x = (s16)pos[0] >> 8;
    s32 y = (s16)pos[2] >> 8;
    s32 cell;
    u8 *p;
    s32 b, h;

    if (x < 0 || y < 0 || (u32)x >= gUnk_030052F8 || (u32)y >= gUnk_030052FC)
        cell = 0;
    else
        cell = *(u16 *)((u8 *)gUnk_030052F4 + 0x24 + y * 2) + x;
    if (cellOut)
        *cellOut = cell;
    p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        p += 4;
    else
        p = (u8 *)gUnk_030052F4[1] + cell * 4 + 0xC;
    b = *p;
    h = (b & 0xF) << 8;
    if ((b >> 4) == 1)
        h -= ((u8 *)pos)[4];
    else if ((b >> 4) == 2)
        h -= ((u8 *)pos)[0];
    return h;
}

#ifdef NONMATCHING
// fuzzy: 31.3%
// scope shape 1525 (differences once low registers are set aside)
// confidence=low: Spider enemy loader (Crow/Ivy-shaped): script params, optional patrol route, callback tables
s32 Spider_Init(u8 *arg0)
{
    u8 *heap;
    u32 zero;
    u32 tmp[2];
    u32 tmp2[2];
    u16 pos[4];
    void *res;
    s32 mode;
    s32 i;
    u32 flag;
    u8 *rt;
    u8 *flagp;
    s32 r4v, r6v, r5v, r2v, r1v;
    s32 extra;

    zero = 0;
    SP_AT(u16, 0x62C) = 0;
    SceneActorRegistry_SetObjectContext(arg0, arg0, 1);
    SP_AT(u8, 0x98) = 2;
    Spider_InitProfile(arg0);

    if (SP_AT(u8, 8) == 0) {
        heap = Heap_Alloc(0x5C);
        SP_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x5C);
        heap = SP_AT(u8 *, 4);
        AnimData_Load(heap + 0x2C, 0x528F);
        Sprite_Init(heap, heap + 0x2C, 0);
        SceneActorAnimation_InitEnemySequence(heap, 0xB6EE);
        if (SP_AT(u8, 0x98) == 0x11 || SP_AT(u8, 0x98) == 0x17)
            heap[7] = 1;
        SP_AT(u8, 0x59) = 0;
    } else {
        heap = Heap_Alloc(0x80);
        SP_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x80);
        heap = SP_AT(u8 *, 4);
        CpuSet(&zero, tmp, 0x05000002);
        res = ScriptProc_Start(0xCB05, 0x528F);
        *(struct Sp32Block *)heap = *(struct Sp32Block *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, tmp);
        SP_AT(u8, 0x2B7) = 0xFF;
    }
    flagp = arg0 + 0x59;

    SP_AT(u32, 0x2CC) = 0;
    SP_AT(u32, 0x2D0) = 0;
    SP_AT(u8, 0x2B4) = 0;
    SP_AT(u16, 0x2E8) = 8;
    SP_AT(u16, 0x2C0) = Script_GetKeywordValue(0x4D, 0);
    SP_AT(u16, 0x2C2) = 0x40;
    CpuSet(&zero, tmp2, 0x05000002);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        for (i = 0; i <= 3; i++) {
            SP_AT(u16, 0xB4 + 2 * i) = SV(0x18);
            SP_AT(u16, 0xBC + 2 * i) = SV(0x18);
            SP_AT(u32, 0xCC + 4 * i) = SV(0);
            SP_AT(u16, 0xC4 + 2 * i) = SV(0);
        }
    }

    if (Script_SeekToKeyword(0x6C)) {
        r5v = SV(0x12C);
        r6v = SV(0x12C);
        r2v = SV(0x12C);
    } else {
        r5v = 0;
        r6v = 0;
        r2v = 0;
    }
    SP_AT(u16, 0x414) = 0;
    SP_AT(u16, 0x416) = r2v;
    SP_AT(u16, 0x418) = 0;
    SP_AT(u16, 0x41A) = 0;
    SP_AT(u32, 0x41C) = 0;

    if (Script_SeekToKeyword(0x6E)) {
        r4v = SV(0);
        SP_AT(u16, 0x420) = SV(0);
    } else {
        r4v = 0;
        SP_AT(u16, 0x420) = r4v;
    }
    SP_AT(u16, 0x422) = r4v;
    SP_AT(u32, 0x424) = SP_AT(u16, 0x422) * SP_AT(u16, 0x422);
    SP_AT(u8, 0x42A) = 0;

    if (Script_SeekToKeyword(0x65)) {
        r4v = SV(0x30);
        r2v = Script_GetPc() ? (u8)Script_GetValue() : 0x40;
        r1v = r4v;
    } else {
        r4v = 0x30;
        r1v = 0x30;
        r2v = 0x40;
    }
    SP_AT(u32, 0xDC) = 0;
    SP_AT(u16, 0xEC) = r4v;
    SP_AT(u16, 0xEE) = r1v;
    SP_AT(u32, 0xF8) = r4v * r4v;
    SP_AT(u32, 0xFC) = r1v * r1v;
    SP_AT(u8, 0xEA) = r2v;
    if (SP_AT(u8, 0xEB) & 4)
        SP_AT(u32, 0xF4) = SP_AT(u32, 0xF8);
    else
        SP_AT(u32, 0xF4) = Div(SP_AT(u32, 0xF8) * 8 - SP_AT(u32, 0xF8), 10);

    if (Script_GetKeywordValue(0x62, 0))
        SP_AT(u32, 0x2D0) |= 0x200;
    SP_AT(u32, 0x1F0) = (u16)Script_GetKeywordValue(0x46, 0);
    SP_AT(u32, 0x1EC) = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        SP_AT(u16, 0xB0) = SV(0);
        SP_AT(s16, 0x5B6) = SP_AT(s16, 0xB0) / 2;
        SP_AT(u8, 0x9E) = SV(0);
        SP_AT(u8, 0x9F) = SV(0);
        SP_AT(u8, 0xA0) = SV(0);
        SP_AT(u16, 0xA2) = SV(0);
        SP_AT(u16, 0xA4) = SV(0);
    }

    r1v = (s16)SP_AT(u16, 0xB0);
    SP_AT(u16, 0x2D4) = 0;
    SP_AT(void *, 0x21C) = SceneActorControl_OnIdle;
    SP_AT(u8, 0x2B2) = 1;
    SP_AT(u8, 0x2B0) = 0;
    if (r1v <= 0)
        r1v = 0;
    SP_AT(u16, 0x9C) = r1v;
    SP_AT(u16, 0xAA) = 0;
    if (r6v <= 0)
        r6v = r5v;
    SP_AT(u16, 0xA6) = r5v;
    SP_AT(u16, 0xA8) = r6v;
    SP_AT(u16, 0x2FA) = 0;

    if (Script_SeekToKeyword(0x58)) {
        SP_AT(u32, 0xAC) = SV(0);
        SP_AT(u8, 0xA1) = SV(0);
    }
    SP_AT(u16, 0xB2) = Script_GetKeywordValue(0x4B, 2);
    r1v = (s16)Script_GetKeywordValue(0x56, -1);
    if (r1v < 0)
        r1v = 0xFFFF;
    SP_AT(u16, 0x9A) = r1v;
    if (Script_GetKeywordValue(0x48, 0))
        SP_AT(u32, 0x2D0) |= 0x800;
    if (Script_GetKeywordValue(0x4C, 0))
        SP_AT(u32, 0x2D0) |= 0x8000;

    SceneBody_Init(arg0 + 0x50, mode, tmp2, 0, 4, arg0);
    rt = arg0 + 0x448;
    rt[0xF] = 0;

    if (Script_SeekToKeyword(0x72)) {
        rt[0x10] = SV(0);
        rt[0x11] = 0;
        r1v = SV(0);
        rt[0x12] = r1v;
        rt[0xC] = r1v;
        rt[0xD + rt[0xF]] = rt[0x10];
        rt[0xD + (rt[0xF] ^ 1)] = Script_GetPc() ? Script_GetValue() : rt[0x10];
        Path_GetPointPositionById(pos, rt[0x10], rt[0x12]);
        SP_AT(s16, 0x2BE) = Spider_CellHeight(pos, &SP_AT(u16, 0x2F4));
        pos[1] = SP_AT(s16, 0x2BE);
        SceneBody_StartRoute(arg0 + 0x50, rt, rt[0x10], rt[0x11], rt[0x12]);
        SP_AT(u16, 0x5C) = **(u16 **)(rt + 8);
        SP_AT(u16, 0x60) = (*(u16 **)(rt + 8))[1];
        SP_AT(s16, 0x5E) = Spider_CellHeight((u16 *)(arg0 + 0x5C), 0);
        if (SP_AT(u8, 8) == 0) {
            *(u32 *)(SP_AT(u8 *, 4) + 0x1C) = SP_AT(u32, 0x5C);
            *(u32 *)(SP_AT(u8 *, 4) + 0x20) = SP_AT(u32, 0x60);
        } else {
            *(u32 *)(SP_AT(u8 *, 4) + 0x40) = SP_AT(u32, 0x5C);
            *(u32 *)(SP_AT(u8 *, 4) + 0x44) = SP_AT(u32, 0x60);
        }
        SP_AT(u8, 0x59) = Route_GetCurrentPointFacing(rt);
    } else {
        rt[0x10] = 0;
        rt[0x11] = 0;
        rt[0x12] = 0;
        rt[0xC] = 0;
        arg0[0x3D4 + 0x81 + rt[0xF]] = 0;
        arg0[0x3D4 + 0x81 + (rt[0xF] ^ 1)] = 0;
    }

    if (SP_AT(u16, 0x2C0) & 0x200)
        TargetRegistry_RegisterActor(arg0 + 0x404, arg0 + 0x50, sub_08080BE0);
    SceneBody_SetSprite(arg0 + 0x50, SP_AT(u8, 8) == 0 ? SP_AT(u8 *, 4) : 0);
    if (SP_AT(u8, 0x98) == 0x15)
        SceneBody_SetMovementProbe(arg0 + 0x50, arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(arg0 + 0x50, arg0 + 0x40, 0x40, 0x40);

    SP_AT(u16, 0x3D4) = SP_AT(u32, 0x50);
    SP_AT(u32, 0x3D8) = (u32)(arg0 + 0x5C);

    SP_AT(u16, 0x1E6) = Script_GetKeywordValue(0x43, 0);
    SP_AT(u16, 0x1E0) = 0xC8;
    SP_AT(u16, 0x1E2) = 0x192;
    SP_AT(u16, 0x1E4) = SP_AT(u16, 0x1E6) + 0xC8;
    r1v = SP_AT(u16, 0x1E0) + SP_AT(u16, 0x1E6);
    if (SP_AT(u8, 8) == 0)
        AnimData_SelectPaletteBank(SP_AT(u8 *, 4) + 0x2C);
    else
        *(u8 **)(SP_AT(u8 *, 4) + 0x68) = gPaletteBankData + (r1v << 5);
    if (SP_AT(u8, 8) == 1)
        *(u16 *)(SP_AT(u8 *, 4) + 0x5A) = SceneActorScript_GetManagerField62() - 0x5FFF;

    Spider_InitColliders(arg0);
    extra = Script_GetKeywordValue(0x66, 0);
    if (Script_GetKeywordValue(0x54, 0))
        SP_AT(u16, 0x62C) |= 0x10;
    SP_AT(u32, 0x2CC) |= 0x2000;
    SP_AT(void *, 0x274) = Spider_OnUpdate;
    SP_AT(void *, 0x260) = Spider_GetAnimationEvent;
    SP_AT(void *, 0x224) = sub_080A21EC;
    SP_AT(void *, 0x228) = Spider_SelectActiveAnimation;
    SP_AT(void *, 0x22C) = Spider_SelectIdleAnimation;
    SP_AT(void *, 0x230) = Spider_UpdateLandingMotion;
    SP_AT(void *, 0x234) = Spider_UpdateDefeat;
    SP_AT(void *, 0x220) = Spider_OnActionEvent;
    SP_AT(void *, 0x278) = SceneActorReaction_OnEnemyDefault;
    SP_AT(u32, 0x270) = 0;
    SP_AT(const void *, 0x250) = gUnk_086108B0;
    SP_AT(u32, 0x248) = 0;
    SP_AT(void *, 0x26C) = SceneActorPlacement_BeginContactRoute;
    SP_AT(void *, 0x238) = SceneActorReaction_SinkAndRemove;
    SP_AT(void *, 0x264) = sub_081363F4;
    SP_AT(void *, 0x268) = SceneActorPlacement_BeginDefaultResponse;
    SP_AT(u32, 0x240) = 0;
    SP_AT(u32, 0x104) = 0;
    SP_AT(u32, 0x108) = 0;
    SP_AT(u32, 0x10C) = 0;
    SP_AT(u32, 0x110) = 0;
    SP_AT(u32, 0x114) = 0;
    SP_AT(u8 *, 0x5D0) = arg0 + 0x630;
    SP_AT(u8 *, 0x5D4) = arg0 + 0x654;
    SP_AT(void *, 0x5F4) = Spider_OnSecondaryColliderContact;
    SP_AT(void *, 0x5F8) = SceneActorReaction_BeginTargetRecovery;
    SP_AT(void *, 0x5FC) = SceneActorReaction_BeginFadedState;
    SP_AT(void *, 0x600) = SceneActorReaction_BeginDefeatBurst;
    SP_AT(u32, 0x43C) = 0;
    SP_AT(u32, 0x440) = 0;
    SP_AT(u8, 0x42B) = 1;
    SP_AT(void *, 0x618) = sub_0807DB34;
    SP_AT(u32, 0x608) = 0;
    SP_AT(u32, 0x61C) = 0;
    SP_AT(u32, 0x620) = 0;
    SP_AT(void *, 0x5F0) = SceneActorReaction_RestoreAfterTurns;
    SP_AT(void *, 0x60C) = sub_0807FCA8;
    SP_AT(u32, 0x604) = 0;
    SP_AT(void *, 0x614) = SceneActorReaction_TurnBackToRoute;
    SP_AT(u32, 0x5E8) = 0;
    SP_AT(void *, 0x444) = sub_0807F130;
    SP_AT(void *, 0x41C) = SceneActorReaction_UpdateAnimationGate;
    SP_AT(u16, 0x41A) = 0x28;
    SP_AT(void *, 0x5DC) = sub_0808170C;
    SP_AT(void *, 0x5E0) = SceneActorReaction_ResumeAfterAnimation;
    SP_AT(void *, 0x5E4) = SceneActorReaction_TryResumeRoute;
    SP_AT(void *, 0x610) = SceneActorReaction_BeginReactionSound;
    ProximityVolume_TranslateBounds(arg0 + 0x630, arg0 + 0x5C);
    SceneActorMovement_ResolveRoutePosition(arg0);
    SP_AT(void *, 0x248) = SceneActorRoute_ResetStep;
    SP_AT(u32, 0x23C) = 0;
    SP_AT(const void *, 0x250) = gUnk_08605F34;
    SP_AT(void *, 0x41C) = SceneActorReaction_ResumeEntryTarget;
    if (SP_AT(u16, 0x1E6) == 0)
        SP_AT(void *, 0x270) = SceneActorPlacement_BeginNearbyPlayerContact;
    else
        SP_AT(void *, 0x270) = SceneActorPlacement_SelectPlayerApproach;
    SP_AT(u8, 0xEB) |= 1;
    SP_AT(u32, 0x2CC) |= 0x400000;
    SP_AT(u32, 0x2D0) |= 8;
    SceneActorTerrain_ReadScriptParameters(arg0);
    r2v = Script_GetKeywordValue(0x75, 0);
    SP_AT(u16, 0x5CC) = 0x21;
    if (r2v != 0) {
        SP_AT(u32, 0x2CC) |= 0x40000;
        if (r2v == 2)
            SP_AT(u32, 0x2CC) |= 0x80000;
    }
    flag = (SP_AT(u16, 0x2C0) & 4) != 0;
    if (flag)
        SP_AT(u32, 0x2D0) |= 0x2000;

    if (!flag) {
        SP_AT(u8, 0x2A8) = 0;
        SP_AT(u8, 0x2A9) = 0;
        SP_AT(u8, 0x2AA) = 0;
        SP_AT(u32, 0x2AC) = 0;
        SP_AT(u8, 0x2B1) = 1;
        SP_AT(u8, 0x2B2) = 1;
        SP_AT(u8, 0x2B0) = 0;
        SP_AT(u8, 0x2B5) = 0;
        SP_AT(u32, 0x21C) = SP_AT(u32, 0x22C);
        SP_AT(u16, 0x2C8) = 0;
        SP_AT(u32, 0x114) &= ~1;
        SP_AT(u32, 0x104) = 0;
    }

    if (SP_AT(u16, 0x2C0) & 2)
        SP_AT(u16, 0x2FA) = *(u32 *)(gUnk_02000710 + 0x5A0);
    else
        SP_AT(u16, 0x2FA) = (u16)(u32)Region_FindContaining(arg0 + 0x5C);

    if (SP_AT(u16, 0x2C0) & 0x40)
        SP_AT(u16, 0x13A) &= 0xFEFF;

    if (Script_GetKeywordValue(0x53, 0) != 0 ||
        *(u32 *)(gUnk_02000710 + 0x5A0) != SP_AT(u16, 0x2FA))
        SceneActorPlacement_HideActor(arg0, 0);

    SceneActorAnimation_SelectDirectionalEntry(arg0, 0);
    if (SP_AT(u8, 8) == 0) {
        u8 *h = SP_AT(u8 *, 4);
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
    } else {
        Anim_Step(SP_AT(u8 *, 4) + 0x20);
    }

    SP_AT(u32, 0x304) = 0;
    SceneActorBehavior_InstallCallbackTables(arg0);
    Spider_SetBehaviorTables(arg0);
    if (extra != 0) {
        SP_AT(u16, 0x2EA) = extra;
        SP_AT(u16, 0x2E8) = 2;
        SP_AT(u8, 0x2B2) = 1;
        SP_AT(u8, 0x2B0) = 0;
        SP_AT(u8, 0x2B5) = 0x23;
        SP_AT(void *, 0x21C) = Spider_UpdateHiddenRecovery;
        SP_AT(u16, 0x2C8) = 0;
        SP_AT(u32, 0x114) &= ~1;
        SP_AT(u32, 0x104) = 0;
        SP_AT(u8, 0x2A8) = 3;
        SP_AT(u8, 0x2A9) = 0;
        SP_AT(u8, 0x2AA) = 0;
        SP_AT(u32, 0x2AC) = 0;
        SP_AT(u8, 0x2B1) = 1;
        ScaledShadow_Initialize(arg0 + 0x360, arg0 + 0x5C, 0x40, 0x10, 0x60, 0x64, 0xA00, 1, 5, 1,
                                0);
        SP_AT(u16, 0x62C) |= 1;
    } else {
        SP_AT(u8, 0x2B1) = 1;
        SP_AT(u8, 0x2B0) = (u8)extra;
        SP_AT(u8, 0x2B5) = 0xE;
        SP_AT(void *, 0x21C) = SceneActorReaction_RestoreAlternateAfterDelay;
        SP_AT(u16, 0x2C8) = (u16)extra;
        SP_AT(u32, 0x114) &= ~1;
        SP_AT(u32, 0x104) = extra;
        SP_AT(u8, 0x3D4 + 0x8C + 8) = 0;
        SP_AT(u8, 0x3D4 + 0x8C + 9) = 0;
        SP_AT(u8, 0x3D4 + 0x8C + 0xA) = 7;
        SP_AT(u32, 0x3D4 + 0x8C + 0xC) = 0x3C;
        SP_AT(u32, 0x3D4 + 0x8C + 0x10) = SP_AT(u32, 0x228);
    }
    SP_AT(u16, 0x62E) = 0x12C;
    SP_AT(u16, 0x12E) = SP_AT(u16, 0x2C0);
    SP_AT(u16, 0x12C) = SP_AT(u16, 0x2EA);
    SP_AT(u32, 0x118) = SP_AT(u32, 0x2CC);
    SP_AT(u32, 0x11C) = SP_AT(u32, 0x2D0);
    SP_AT(u8, 0x120) = *flagp;
    SP_AT(u8, 0x121) = SP_AT(u8, 0x2A8);
    SP_AT(u8, 0x122) = SP_AT(u8, 0x2A9);
    SP_AT(u8, 0x123) = SP_AT(u8, 0x2AA);
    SP_AT(u32, 0x124) = SP_AT(u32, 0x21C);
    SP_AT(u8, 0x128) = SP_AT(u8, 0x2B5);
    SP_AT(void *, 0x24C) = SceneActorScript_OnCommand;
    SP_AT(void *, 0x258) = Spider_OnSecondaryUpdate;
    SP_AT(void *, 0x25C) = Spider_Destroy;

    return SceneActorRegistry_LinkActor(arg0) ? 0 : -1;
}
#else
INCLUDE_ASM("asm/nonmatching", Spider_Init);
#endif
