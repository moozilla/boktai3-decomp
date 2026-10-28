#include "global.h"
#include "render.h"

#define SK_AT(T, off) (*(T *)((u8 *)arg0 + (off)))
#define SV(d)         (Script_GetPc() ? Script_GetValue() : (d))

struct Skel32Block {
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
void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);
void sub_08080BE0(void);
void SceneActorReaction_SinkAndRemove(void);
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
void SceneActorReaction_OnEnemyDefault(void);
void sub_081363F4(void);
void SceneActorScript_OnCommand(void);
void SceneActorControl_OnIdle(void);
void SceneActorReaction_PlayHiddenCue(void);
extern const u8 gUnk_086108B0[];
extern const u8 gUnk_08606400[];
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

extern u32 *gTerrainMap;
extern u32 gTerrainWidth;
extern u32 gTerrainHeight;
extern u8 *gGameState;

/* terrain height probe: (x, y) world pos -> cell id -> height offset */
static inline s32 Skel_CellHeight(u16 *pos, u16 *cellOut)
{
    s32 x = (s16)pos[0] >> 8;
    s32 y = (s16)pos[2] >> 8;
    s32 cell;
    u8 *p;
    s32 b, h;

    if (x < 0 || y < 0 || (u32)x >= gTerrainWidth || (u32)y >= gTerrainHeight)
        cell = 0;
    else
        cell = *(u16 *)((u8 *)gTerrainMap + 0x24 + y * 2) + x;
    if (cellOut)
        *cellOut = cell;
    p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        p += 4;
    else
        p = (u8 *)gTerrainMap[1] + cell * 4 + 0xC;
    b = *p;
    h = (b & 0xF) << 8;
    if ((b >> 4) == 1)
        h -= ((u8 *)pos)[4];
    else if ((b >> 4) == 2)
        h -= ((u8 *)pos)[0];
    return h;
}

void SceneActorBehavior_InitializeShadow(void *);
void SpriteGroup_InitAnimatedSprites(void *);
void ActorCollision_InitThreeAuxiliaryAttacks(void *);
void ActorGroup_ReadScriptOptions(void *);
extern u8 *gUnknown_02000580;

void SceneActor_UpdateProfileForCategory(void *);
void SceneActorReaction_ParseOptions(void *);
void SceneActorScript_ReadPParameters(void *);
void SceneActorScript_ReadCallScriptConfig(void *);
void SceneActorControl_InitBehaviorColliders(void *);
void ProximityVolume_TranslateBounds(void *, void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorRegistry_RegisterCategoryReceiver(void *);
void Skeleton_InitVariantMotion(void *);
void Skeleton_SetMovementControlPreset(void *);
void Skeleton_SetAttackControlTable(void *);
void Skeleton_SetReactionControlTable(void *);
void Skeleton_Update(void);
void SceneActorReaction_SelectTileParameter(void);
void sub_080BAE04(void);
void Skeleton_UpdateInactiveState(void);
void Skeleton_UpdateIdleState(void);
void Skeleton_ConvergeDisplacedPosition(void);
void Skeleton_UpdateDefeat(void);
void sub_0807F81C(void);
void TentacleBody_UpdateReactionTransition(void);
void SceneActorTarget_ResolvePendingObjectSkeleton(void);
void SceneActorPlacement_ResetStateThree(void);
void Skeleton_UpdateDownedState(void);
void SceneActorReaction_ReturnToRouteEntry(void);
void SceneActorReaction_BeginTargetRecovery(void);
void Skeleton_ResetRouteReaction(void);
void SceneActorReaction_UpdateGate(void);
void Skeleton_RestorePursuitRoute(void);
void Golem_ReactToContact(void);
void SceneActorReaction_SinkThenDefeat(void);
void Skeleton_BeginKnockedFlight(void);
void SceneActorReaction_TickHiddenTimeout(void);
void Skeleton_UpdateBehaviorCallbacks(void);
void Skeleton_Destroy(void);
extern const u8 gUnk_086060FC[];

#ifdef NONMATCHING
// fuzzy: 30.4%
// scope shape 1607 (differences once low registers are set aside)
// confidence=low: Skeleton enemy loader (Ivy/Axe-shaped): script params, optional patrol route, callback tables
s32 Skeleton_Init(u8 *arg0)
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

    zero = 0;
    SK_AT(u32, 0x6DC) = 0;
    SceneActorRegistry_SetObjectContext(arg0, arg0, 1, 1);
    SK_AT(u8, 0x98) = (u8)Script_GetKeywordValue(0x6B, 5);
    if (SK_AT(u8, 0x98) == 7)
        SK_AT(u32, 0x6DC) |= 0x40;
    SceneActor_UpdateProfileForCategory(arg0);

    if (SK_AT(u8, 8) == 0) {
        heap = Heap_Alloc(0x5C);
        SK_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x5C);
        heap = SK_AT(u8 *, 4);
        AnimData_Load(heap + 0x2C, 0xAE52);
        Sprite_Init(heap, heap + 0x2C, 0);
        SceneActorAnimation_InitEnemySequence(heap, 0);
        if (SK_AT(u8, 0x98) == 0x11 || SK_AT(u8, 0x98) == 0x17)
            heap[7] = 1;
        SK_AT(u8, 0x59) = 0;
    } else {
        heap = Heap_Alloc(0x80);
        SK_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x80);
        heap = SK_AT(u8 *, 4);
        CpuSet(&zero, tmp, 0x05000002);
        res = ScriptProc_Start(0xCB05, 0xAE52);
        *(struct Skel32Block *)heap = *(struct Skel32Block *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, tmp);
        SK_AT(u8, 0x2B7) = 0xFF;
    }
    flagp = arg0 + 0x59;

    SK_AT(u32, 0x2CC) = 0;
    SK_AT(u32, 0x2D0) = 0;
    SK_AT(u8, 0x2B4) = 0;
    SK_AT(u16, 0x2E8) = 8;
    SK_AT(u16, 0x2C0) = Script_GetKeywordValue(0x4D, 0);
    SK_AT(u16, 0x2C2) = 0x40;
    CpuSet(&zero, tmp2, 0x05000002);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        for (i = 0; i <= 3; i++) {
            SK_AT(u16, 0xB4 + 2 * i) = SV(0x18);
            SK_AT(u16, 0xBC + 2 * i) = SV(0x18);
            SK_AT(u32, 0xCC + 4 * i) = SV(0);
            SK_AT(u16, 0xC4 + 2 * i) = SV(0);
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
    SK_AT(u16, 0x414) = 0;
    SK_AT(u16, 0x416) = r2v;
    SK_AT(u16, 0x418) = 0;
    SK_AT(u16, 0x41A) = 0;
    SK_AT(u32, 0x41C) = 0;

    if (Script_SeekToKeyword(0x6E)) {
        r4v = SV(0);
        SK_AT(u16, 0x420) = SV(0);
    } else {
        r4v = 0;
        SK_AT(u16, 0x420) = r4v;
    }
    SK_AT(u16, 0x422) = r4v;
    SK_AT(u32, 0x424) = SK_AT(u16, 0x422) * SK_AT(u16, 0x422);
    SK_AT(u8, 0x42A) = 0;

    if (Script_SeekToKeyword(0x65)) {
        r4v = SV(0x30);
        r2v = Script_GetPc() ? (u8)Script_GetValue() : 0x40;
        r1v = r4v;
    } else {
        r4v = 0x30;
        r1v = 0x30;
        r2v = 0x40;
    }
    SK_AT(u32, 0xDC) = 0;
    SK_AT(u16, 0xEC) = r4v;
    SK_AT(u16, 0xEE) = r1v;
    SK_AT(u32, 0xF8) = r4v * r4v;
    SK_AT(u32, 0xFC) = r1v * r1v;
    SK_AT(u8, 0xEA) = r2v;
    if (SK_AT(u8, 0xEB) & 4)
        SK_AT(u32, 0xF4) = SK_AT(u32, 0xF8);
    else
        SK_AT(u32, 0xF4) = Div(SK_AT(u32, 0xF8) * 8 - SK_AT(u32, 0xF8), 10);

    if (Script_GetKeywordValue(0x62, 0))
        SK_AT(u32, 0x2D0) |= 0x200;
    SK_AT(u32, 0x1F0) = (u16)Script_GetKeywordValue(0x46, 0);
    SK_AT(u32, 0x1EC) = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        SK_AT(u16, 0xB0) = SV(0);
        SK_AT(s16, 0x5B6) = SK_AT(s16, 0xB0) / 2;
        SK_AT(u8, 0x9E) = SV(0);
        SK_AT(u8, 0x9F) = SV(0);
        SK_AT(u8, 0xA0) = SV(0);
        SK_AT(u16, 0xA2) = SV(0);
        SK_AT(u16, 0xA4) = SV(0);
    }

    r1v = (s16)SK_AT(u16, 0xB0);
    SK_AT(u16, 0x2D4) = 0;
    SK_AT(void *, 0x21C) = SceneActorControl_OnIdle;
    SK_AT(u8, 0x2B2) = 1;
    SK_AT(u8, 0x2B0) = 0;
    if (r1v <= 0)
        r1v = 0;
    SK_AT(u16, 0x9C) = r1v;
    SK_AT(u16, 0xAA) = 0;
    if (r6v <= 0)
        r6v = r5v;
    SK_AT(u16, 0xA6) = r5v;
    SK_AT(u16, 0xA8) = r6v;
    SK_AT(u16, 0x2FA) = 0;

    if (Script_SeekToKeyword(0x58)) {
        SK_AT(u32, 0xAC) = SV(0);
        SK_AT(u8, 0xA1) = SV(0);
    }
    SK_AT(u16, 0xB2) = Script_GetKeywordValue(0x4B, 2);
    r1v = (s16)Script_GetKeywordValue(0x56, -1);
    if (r1v < 0)
        r1v = 0xFFFF;
    SK_AT(u16, 0x9A) = r1v;
    if (Script_GetKeywordValue(0x48, 0))
        SK_AT(u32, 0x2D0) |= 0x800;
    if (Script_GetKeywordValue(0x4C, 0))
        SK_AT(u32, 0x2D0) |= 0x8000;

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
        SK_AT(s16, 0x2BE) = Skel_CellHeight(pos, &SK_AT(u16, 0x2F4));
        pos[1] = SK_AT(s16, 0x2BE);
        SceneBody_StartRoute(arg0 + 0x50, rt, rt[0x10], rt[0x11], rt[0x12]);
        SK_AT(u16, 0x5C) = **(u16 **)(rt + 8);
        SK_AT(u16, 0x60) = (*(u16 **)(rt + 8))[1];
        SK_AT(s16, 0x5E) = Skel_CellHeight((u16 *)(arg0 + 0x5C), 0);
        if (SK_AT(u8, 8) == 0) {
            *(u32 *)(SK_AT(u8 *, 4) + 0x1C) = SK_AT(u32, 0x5C);
            *(u32 *)(SK_AT(u8 *, 4) + 0x20) = SK_AT(u32, 0x60);
        } else {
            *(u32 *)(SK_AT(u8 *, 4) + 0x40) = SK_AT(u32, 0x5C);
            *(u32 *)(SK_AT(u8 *, 4) + 0x44) = SK_AT(u32, 0x60);
        }
        SK_AT(u8, 0x59) = Route_GetCurrentPointFacing(rt);
    } else {
        rt[0x10] = 0;
        rt[0x11] = 0;
        rt[0x12] = 0;
        rt[0xC] = 0;
        arg0[0x3D4 + 0x81 + rt[0xF]] = 0;
        arg0[0x3D4 + 0x81 + (rt[0xF] ^ 1)] = 0;
    }

    if (SK_AT(u16, 0x2C0) & 0x200)
        TargetRegistry_RegisterActor(arg0 + 0x404, arg0 + 0x50, sub_08080BE0);
    SceneBody_SetSprite(arg0 + 0x50, SK_AT(u8, 8) == 0 ? SK_AT(u8 *, 4) : 0);
    if (SK_AT(u8, 0x98) == 0x15)
        SceneBody_SetMovementProbe(arg0 + 0x50, arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(arg0 + 0x50, arg0 + 0x40, 0x40, 0x40);

    SK_AT(u16, 0x3D4) = SK_AT(u32, 0x50);
    SK_AT(u32, 0x3D8) = (u32)(arg0 + 0x5C);
    SceneActorReaction_ParseOptions(arg0);
    SceneActorScript_ReadPParameters(arg0);
    SceneActorScript_ReadCallScriptConfig(arg0);

    SK_AT(u16, 0x1E6) = Script_GetKeywordValue(0x43, 0);
    SK_AT(u16, 0x1E0) = 0x13C;
    SK_AT(u16, 0x1E2) = 0x146;
    SK_AT(u16, 0x1E4) = SK_AT(u16, 0x1E6) + 0x13C;
    r1v = SK_AT(u16, 0x1E0) + SK_AT(u16, 0x1E6);
    if (SK_AT(u8, 8) == 0)
        AnimData_SelectPaletteBank(SK_AT(u8 *, 4) + 0x2C);
    else
        *(u8 **)(SK_AT(u8 *, 4) + 0x68) = gPaletteBankData + (r1v << 5);
    if (SK_AT(u8, 8) == 1)
        *(u16 *)(SK_AT(u8 *, 4) + 0x5A) = SceneActorScript_GetManagerField62() - 0x5FFF;
    SceneActorControl_InitBehaviorColliders(arg0);

    SK_AT(u32, 0x2CC) |= 0x2000;
    SK_AT(void *, 0x274) = Skeleton_Update;
    SK_AT(void *, 0x260) = SceneActorReaction_SelectTileParameter;
    SK_AT(void *, 0x224) = sub_080BAE04;
    SK_AT(void *, 0x228) = Skeleton_UpdateInactiveState;
    SK_AT(void *, 0x22C) = Skeleton_UpdateIdleState;
    SK_AT(void *, 0x230) = Skeleton_ConvergeDisplacedPosition;
    SK_AT(void *, 0x234) = Skeleton_UpdateDefeat;
    SK_AT(void *, 0x220) = TentacleBody_UpdateReactionTransition;
    SK_AT(void *, 0x278) = SceneActorReaction_OnEnemyDefault;
    SK_AT(void *, 0x270) = sub_0807F81C;
    SK_AT(const void *, 0x250) = gUnk_086108B0;
    SK_AT(u32, 0x248) = 0;
    SK_AT(void *, 0x26C) = SceneActorTarget_ResolvePendingObjectSkeleton;
    SK_AT(void *, 0x238) = SceneActorReaction_SinkAndRemove;
    SK_AT(void *, 0x264) = sub_081363F4;
    SK_AT(void *, 0x268) = SceneActorPlacement_ResetStateThree;
    SK_AT(u32, 0x240) = 0;
    SK_AT(u32, 0x104) = 0;
    SK_AT(u32, 0x108) = 0;
    SK_AT(u32, 0x10C) = 0;
    SK_AT(u32, 0x110) = 0;
    SK_AT(u32, 0x114) = 0;
    SK_AT(u8 *, 0x5D0) = arg0 + 0x71C;
    SK_AT(u8 *, 0x5D4) = arg0 + 0x740;
    SK_AT(void *, 0x5F4) = Skeleton_UpdateDownedState;
    SK_AT(void *, 0x5F8) = SceneActorReaction_BeginTargetRecovery;
    SK_AT(void *, 0x5FC) = SceneActorReaction_BeginFadedState;
    SK_AT(void *, 0x600) = SceneActorReaction_BeginDefeatBurst;
    SK_AT(void *, 0x43C) = SceneActorReaction_ReturnToRouteEntry;
    SK_AT(u32, 0x440) = 0;
    SK_AT(u8, 0x42B) = 1;
    SK_AT(void *, 0x618) = sub_0807DB34;
    SK_AT(u32, 0x608) = 0;
    SK_AT(u32, 0x61C) = 0;
    SK_AT(u32, 0x620) = 0;
    SK_AT(void *, 0x5F0) = SceneActorReaction_RestoreAfterTurns;
    SK_AT(void *, 0x60C) = sub_0807FCA8;
    SK_AT(u32, 0x604) = 0;
    SK_AT(void *, 0x614) = SceneActorReaction_TurnBackToRoute;
    SK_AT(u32, 0x5E8) = 0;
    SK_AT(void *, 0x444) = sub_0807F130;
    SK_AT(void *, 0x41C) = SceneActorReaction_UpdateAnimationGate;
    SK_AT(u16, 0x41A) = 0x28;
    SK_AT(void *, 0x5DC) = sub_0808170C;
    SK_AT(void *, 0x5E0) = SceneActorReaction_ResumeAfterAnimation;
    SK_AT(void *, 0x5E4) = SceneActorReaction_TryResumeRoute;
    SK_AT(void *, 0x610) = SceneActorReaction_BeginReactionSound;
    ProximityVolume_TranslateBounds(arg0 + 0x71C, arg0 + 0x5C);
    SceneActorMovement_ResolveRoutePosition(arg0);
    SK_AT(void *, 0x248) = Skeleton_ResetRouteReaction;
    SK_AT(u32, 0x61C) = 0;
    SK_AT(void *, 0x23C) = SceneActorReaction_UpdateGate;
    SK_AT(void *, 0x5EC) = Skeleton_RestorePursuitRoute;
    SK_AT(void *, 0x268) = Golem_ReactToContact;
    SK_AT(const void *, 0x250) = gUnk_086060FC;
    SK_AT(void *, 0x238) = SceneActorReaction_SinkThenDefeat;
    SK_AT(u16, 0x2E8) = 0;
    SK_AT(u32, 0x718) = (s16)SK_AT(u16, 0x9C) >> 1;
    SK_AT(u8, 0xDC + 0xF) |= 1;
    SK_AT(u32, 0x2CC) |= 0x1600000;
    SK_AT(u32, 0x2D0) |= 0x8000008;
    SK_AT(u16, 0x2C0) |= 0x80;
    SK_AT(u16, 0x13A) &= ~1;
    SceneActorBehavior_InitializeShadow(arg0);
    SceneActorTerrain_ReadScriptParameters(arg0);
    i = Script_GetKeywordValue(0x75, 0);
    SK_AT(u16, 0x5CC) = 0x21;
    if (i != 0) {
        SK_AT(u32, 0x2CC) |= 0x40000;
        if (i == 2)
            SK_AT(u32, 0x2CC) |= 0x80000;
    }
    flag = (SK_AT(u16, 0x2C0) & 4) != 0;
    if (flag)
        SK_AT(u32, 0x2D0) |= 0x2000;

    if (!flag) {
        SK_AT(u8, 0x2A8) = 0;
        SK_AT(u8, 0x2A9) = 0;
        SK_AT(u8, 0x2AA) = 0;
        SK_AT(u32, 0x2AC) = 0;
        SK_AT(u8, 0x2B1) = 1;
        SK_AT(u8, 0x2B2) = 1;
        SK_AT(u8, 0x2B0) = 0;
        SK_AT(u8, 0x2B5) = 0;
        SK_AT(u32, 0x21C) = SK_AT(u32, 0x22C);
        SK_AT(u16, 0x2C8) = 0;
        SK_AT(u32, 0x114) &= ~1;
        SK_AT(u32, 0x104) = 0;
    }

    if (SK_AT(u16, 0x2C0) & 2)
        SK_AT(u16, 0x2FA) = *(u32 *)(gGameState + 0x5A0);
    else
        SK_AT(u16, 0x2FA) = (u16)(u32)Region_FindContaining(arg0 + 0x5C);

    if (SK_AT(u16, 0x2C0) & 0x40)
        SK_AT(u16, 0x13A) &= 0xFEFF;

    if (Script_GetKeywordValue(0x53, 0) != 0 || *(u32 *)(gGameState + 0x5A0) != SK_AT(u16, 0x2FA))
        SceneActorPlacement_HideActor(arg0, 0);

    SceneActorAnimation_SelectDirectionalEntry(arg0, 0);
    if (SK_AT(u8, 8) == 0) {
        u8 *h = SK_AT(u8 *, 4);
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
        Anim_Step(SK_AT(u8 *, 4) + 0x20);
    }

    SK_AT(u32, 0x304) = 0;
    SceneActorRegistry_RegisterCategoryReceiver(arg0);
    Skeleton_InitVariantMotion(arg0);
    SceneActorBehavior_InstallCallbackTables(arg0);
    Skeleton_SetMovementControlPreset(arg0);
    Skeleton_SetAttackControlTable(arg0);
    Skeleton_SetReactionControlTable(arg0);

    SK_AT(u16, 0x12E) = SK_AT(u16, 0x2C0);
    SK_AT(u16, 0x12C) = SK_AT(u16, 0x2EA);
    SK_AT(u32, 0x118) = SK_AT(u32, 0x2CC);
    SK_AT(u32, 0x11C) = SK_AT(u32, 0x2D0);
    SK_AT(u8, 0x120) = *flagp;
    SK_AT(u8, 0x121) = SK_AT(u8, 0x2A8);
    SK_AT(u8, 0x122) = SK_AT(u8, 0x2A9);
    SK_AT(u8, 0x123) = SK_AT(u8, 0x2AA);
    SK_AT(u32, 0x124) = SK_AT(u32, 0x21C);
    SK_AT(u8, 0x128) = SK_AT(u8, 0x2B5);
    SK_AT(u32, 0x130) = SK_AT(u32, 0x6DC);

    if (SK_AT(u32, 0x6DC) & 0x800) {
        SK_AT(u8, 0x2B2) = 1;
        SK_AT(u8, 0x2B0) = 0;
        SK_AT(u8, 0x2B5) = 0x3D;
        SK_AT(void *, 0x21C) = Skeleton_BeginKnockedFlight;
        SK_AT(u16, 0x2C8) = 0;
        SK_AT(u32, 0x114) &= ~1;
        SK_AT(u32, 0x104) = 0;
        SK_AT(u8, 0x2A8) = 3;
        SK_AT(u8, 0x2A9) = 0;
        SK_AT(u8, 0x2AA) = 0;
        SK_AT(u32, 0x2AC) = 0;
        SK_AT(u8, 0x2B1) = 1;
        SK_AT(u32, 0x6DC) |= 0x10;
        SK_AT(u16, 0x2C0) |= 0x1800;
        if (Script_SeekToKeyword(0x70)) {
            SK_AT(u16, 0x5C) = SV(0);
            SK_AT(u16, 0x5E) = SV(0);
            SK_AT(u16, 0x60) = SV(0);
            SK_AT(u8, 0x59) = SV(0);
            *(u16 *)(SK_AT(u8 *, 4) + 0x20 + 0x3A) = SV(0) + 0x12B;
            SK_AT(s16, 0x2BE) = Skel_CellHeight((u16 *)(arg0 + 0x5C), 0);
            SK_AT(u16, 0x2EA) = SK_AT(u16, 0x5E) - SK_AT(s16, 0x2BE);
            SK_AT(u16, 0x5E) = SK_AT(u16, 0x2BE);
        }
    } else if (SK_AT(u32, 0x6DC) & 8) {
        SK_AT(u8, 0x2B2) = 1;
        SK_AT(u8, 0x2B0) = 0;
        SK_AT(u8, 0x2B5) = 0x39;
        SK_AT(void *, 0x21C) = SceneActorReaction_TickHiddenTimeout;
        SK_AT(u16, 0x2C8) = 0;
        SK_AT(u32, 0x114) &= ~1;
        SK_AT(u32, 0x104) = 0;
        SK_AT(u8, 0x2A8) = 3;
        SK_AT(u8, 0x2A9) = 0;
        SK_AT(u8, 0x2AA) = 0;
        SK_AT(u32, 0x2AC) = 0;
        SK_AT(u8, 0x2B1) = 1;
        SK_AT(u16, 0x2C0) |= 0x1800;
        SK_AT(u32, 0x6DC) |= 0x10;
    }
    if (SK_AT(u8, 0x98) == 8)
        SK_AT(u16, 0x2C0) |= 0x800;

    SK_AT(void *, 0x24C) = SceneActorScript_OnCommand;
    SK_AT(void *, 0x258) = Skeleton_UpdateBehaviorCallbacks;
    SK_AT(void *, 0x25C) = Skeleton_Destroy;

    return SceneActorRegistry_LinkActor(arg0) ? 0 : -1;
}
#else
INCLUDE_ASM("asm/nonmatching", Skeleton_Init);
#endif
