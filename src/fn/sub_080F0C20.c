#include "global.h"
#include "render.h"
#include "engine_globals.h"

void sub_08081290(void);
void sub_08080868(void);
void sub_08080494(void);

void sub_08082444(void);

void sub_08080AF0(void);

void sub_08080008(void);

void sub_08081B9C(void);
void sub_08082184(void);
void sub_08080984(void);
void sub_0812EED4(void);

void sub_08136F1C(void);
void sub_0807FC90(void);
void sub_080ED3FC(void);

/* terrain height probe: (x, y) world pos -> cell id -> height offset */

// confidence=low: Ivy enemy loader (Crow-shaped): script params, optional patrol route, callback tables

void Ivy_UpdateDefense(void);

void sub_080F0325(void);

/* terrain height probe: (x, y) world pos -> cell id -> height offset */

// confidence=low: Ivy enemy loader (Crow-shaped): script params, optional patrol route, callback tables

/* terrain height probe: (x, y) world pos -> cell id -> height offset */

// confidence=low: Ivy enemy loader (Crow-shaped): script params, optional patrol route, callback tables

/* terrain height probe: (x, y) world pos -> cell id -> height offset */

// confidence=low: Ivy enemy loader (Crow-shaped): script params, optional patrol route, callback tables
#define IVY_AT(T, off) (*(T *)((u8 *)arg0 + (off)))
#define SV(d)          (Script_GetPc() ? Script_GetValue() : (d))

struct Ivy32Block {
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
void Ivy_InitProfileId(void *);
void Ivy_InitControlPresets(void *);
void Ivy_Update(void);
void Ivy_UpdateDefeat(void);
void Ivy_TickDrawable(void);
void Ivy_UpdateInteraction(void);
void Ivy_Destroy(void);
void SceneActorControl_BeginSoundTransition(void);
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
void SceneActor_InitCollision5005(void *);
void SceneActor_BeginReaction22(void *);
void SceneActorPlacement_HideActor(void *, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_Step(void *);
void SceneActorBehavior_InstallCallbackTables(void *);
void SceneActorTerrain_ReadScriptParameters(void *);
s32 SceneActorRegistry_LinkActor(void *);
void *Region_FindContaining(void *);

extern u8 *gGameState;

/* terrain height probe: (x, y) world pos -> cell id -> height offset */
static inline s32 Ivy_CellHeight(u16 *pos, u16 *cellOut)
{
    s32 x = (s16)pos[0] >> 8;
    s32 y = (s16)pos[2] >> 8;
    s32 cell;
    u8 *p;
    s32 b, h;

    if (x < 0 || y < 0 || (u32)x >= gTerrainWidth || (u32)y >= gTerrainHeight)
        cell = 0;
    else
        cell = *(u16 *)((u8 *)((u32 *)gTerrainMap) + 0x24 + y * 2) + x;
    if (cellOut)
        *cellOut = cell;
    p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        p += 4;
    else
        p = (u8 *)((u32 *)gTerrainMap)[1] + cell * 4 + 0xC;
    b = *p;
    h = (b & 0xF) << 8;
    if ((b >> 4) == 1)
        h -= ((u8 *)pos)[4];
    else if ((b >> 4) == 2)
        h -= ((u8 *)pos)[0];
    return h;
}

#ifdef NONMATCHING
// fuzzy: 31.4%
// scope shape 1412 (differences once low registers are set aside)
// confidence=low: Ivy enemy loader (Crow-shaped): script params, optional patrol route, callback tables
s32 sub_080F0C20(u8 *arg0)
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
    IVY_AT(u32, 0x62C) = 0;
    SceneActorRegistry_SetObjectContext(arg0, arg0, 1);
    IVY_AT(u8, 0x98) = 0x18;
    Ivy_InitProfileId(arg0);

    if (IVY_AT(u8, 8) == 0) {
        heap = Heap_Alloc(0x5C);
        IVY_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x5C);
        heap = IVY_AT(u8 *, 4);
        AnimData_Load(heap + 0x2C, 0xD638);
        Sprite_Init(heap, heap + 0x2C, 0);
        SceneActorAnimation_InitEnemySequence(heap, 0x99DD);
        if (IVY_AT(u8, 0x98) == 0x11 || IVY_AT(u8, 0x98) == 0x17)
            heap[7] = 1;
        IVY_AT(u8, 0x59) = 0;
    } else {
        heap = Heap_Alloc(0x80);
        IVY_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x80);
        heap = IVY_AT(u8 *, 4);
        CpuSet(&zero, tmp, 0x05000002);
        res = ScriptProc_Start(0xCB05, 0xD638);
        *(struct Ivy32Block *)heap = *(struct Ivy32Block *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, tmp);
        IVY_AT(u8, 0x2B7) = 0xFF;
    }
    flagp = arg0 + 0x59;

    IVY_AT(u32, 0x2CC) = 0;
    IVY_AT(u32, 0x2D0) = 0;
    IVY_AT(u8, 0x2B4) = 0;
    IVY_AT(u16, 0x2E8) = 8;
    IVY_AT(u16, 0x2C0) = Script_GetKeywordValue(0x4D, 0);
    IVY_AT(u16, 0x2C2) = 0x40;
    CpuSet(&zero, tmp2, 0x05000002);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        for (i = 0; i <= 3; i++) {
            IVY_AT(u16, 0xB4 + 2 * i) = SV(0x18);
            IVY_AT(u16, 0xBC + 2 * i) = SV(0x18);
            IVY_AT(u32, 0xCC + 4 * i) = SV(0);
            IVY_AT(u16, 0xC4 + 2 * i) = SV(0);
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
    IVY_AT(u16, 0x414) = 0;
    IVY_AT(u16, 0x416) = r2v;
    IVY_AT(u16, 0x418) = 0;
    IVY_AT(u16, 0x41A) = 0;
    IVY_AT(u32, 0x41C) = 0;

    if (Script_SeekToKeyword(0x6E)) {
        r4v = SV(0);
        IVY_AT(u16, 0x420) = SV(0);
    } else {
        r4v = 0;
        IVY_AT(u16, 0x420) = r4v;
    }
    IVY_AT(u16, 0x422) = r4v;
    IVY_AT(u32, 0x424) = IVY_AT(u16, 0x422) * IVY_AT(u16, 0x422);
    IVY_AT(u8, 0x42A) = 0;

    if (Script_SeekToKeyword(0x65)) {
        r4v = SV(0x30);
        r2v = Script_GetPc() ? (u8)Script_GetValue() : 0x40;
        r1v = r4v;
    } else {
        r4v = 0x30;
        r1v = 0x30;
        r2v = 0x40;
    }
    IVY_AT(u32, 0xDC) = 0;
    IVY_AT(u16, 0xEC) = r4v;
    IVY_AT(u16, 0xEE) = r1v;
    IVY_AT(u32, 0xF8) = r4v * r4v;
    IVY_AT(u32, 0xFC) = r1v * r1v;
    IVY_AT(u8, 0xEA) = r2v;
    if (IVY_AT(u8, 0xEB) & 4)
        IVY_AT(u32, 0xF4) = IVY_AT(u32, 0xF8);
    else
        IVY_AT(u32, 0xF4) = Div(IVY_AT(u32, 0xF8) * 8 - IVY_AT(u32, 0xF8), 10);

    if (Script_GetKeywordValue(0x62, 0))
        IVY_AT(u32, 0x2D0) |= 0x200;
    IVY_AT(u32, 0x1F0) = (u16)Script_GetKeywordValue(0x46, 0);
    IVY_AT(u32, 0x1EC) = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        IVY_AT(u16, 0xB0) = SV(0);
        IVY_AT(s16, 0x5B6) = IVY_AT(s16, 0xB0) / 2;
        IVY_AT(u8, 0x9E) = SV(0);
        IVY_AT(u8, 0x9F) = SV(0);
        IVY_AT(u8, 0xA0) = SV(0);
        IVY_AT(u16, 0xA2) = SV(0);
        IVY_AT(u16, 0xA4) = SV(0);
    }

    r1v = (s16)IVY_AT(u16, 0xB0);
    IVY_AT(u16, 0x2D4) = 0;
    IVY_AT(void *, 0x21C) = SceneActorControl_OnIdle;
    IVY_AT(u8, 0x2B2) = 1;
    IVY_AT(u8, 0x2B0) = 0;
    if (r1v <= 0)
        r1v = 0;
    IVY_AT(u16, 0x9C) = r1v;
    IVY_AT(u16, 0xAA) = 0;
    if (r6v <= 0)
        r6v = r5v;
    IVY_AT(u16, 0xA6) = r5v;
    IVY_AT(u16, 0xA8) = r6v;
    IVY_AT(u16, 0x2FA) = 0;

    if (Script_SeekToKeyword(0x58)) {
        IVY_AT(u32, 0xAC) = SV(0);
        IVY_AT(u8, 0xA1) = SV(0);
    }
    IVY_AT(u16, 0xB2) = Script_GetKeywordValue(0x4B, 2);
    r1v = (s16)Script_GetKeywordValue(0x56, -1);
    if (r1v < 0)
        r1v = 0xFFFF;
    IVY_AT(u16, 0x9A) = r1v;
    if (Script_GetKeywordValue(0x48, 0))
        IVY_AT(u32, 0x2D0) |= 0x800;
    if (Script_GetKeywordValue(0x4C, 0))
        IVY_AT(u32, 0x2D0) |= 0x8000;

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
        IVY_AT(s16, 0x2BE) = Ivy_CellHeight(pos, &IVY_AT(u16, 0x2F4));
        pos[1] = IVY_AT(s16, 0x2BE);
        SceneBody_StartRoute(arg0 + 0x50, rt, rt[0x10], rt[0x11], rt[0x12]);
        IVY_AT(u16, 0x5C) = **(u16 **)(rt + 8);
        IVY_AT(u16, 0x60) = (*(u16 **)(rt + 8))[1];
        IVY_AT(s16, 0x5E) = Ivy_CellHeight((u16 *)(arg0 + 0x5C), 0);
        if (IVY_AT(u8, 8) == 0) {
            *(u32 *)(IVY_AT(u8 *, 4) + 0x1C) = IVY_AT(u32, 0x5C);
            *(u32 *)(IVY_AT(u8 *, 4) + 0x20) = IVY_AT(u32, 0x60);
        } else {
            *(u32 *)(IVY_AT(u8 *, 4) + 0x40) = IVY_AT(u32, 0x5C);
            *(u32 *)(IVY_AT(u8 *, 4) + 0x44) = IVY_AT(u32, 0x60);
        }
        IVY_AT(u8, 0x59) = Route_GetCurrentPointFacing(rt);
    } else {
        rt[0x10] = 0;
        rt[0x11] = 0;
        rt[0x12] = 0;
        rt[0xC] = 0;
        arg0[0x3D4 + 0x81 + rt[0xF]] = 0;
        arg0[0x3D4 + 0x81 + (rt[0xF] ^ 1)] = 0;
    }

    if (IVY_AT(u16, 0x2C0) & 0x200)
        TargetRegistry_RegisterActor(arg0 + 0x404, arg0 + 0x50, sub_08080BE0);
    SceneBody_SetSprite(arg0 + 0x50, IVY_AT(u8, 8) == 0 ? IVY_AT(u8 *, 4) : 0);
    if (IVY_AT(u8, 0x98) == 0x15)
        SceneBody_SetMovementProbe(arg0 + 0x50, arg0 + 0x40, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(arg0 + 0x50, arg0 + 0x40, 0x40, 0x40);

    IVY_AT(u16, 0x3D4) = IVY_AT(u32, 0x50);
    IVY_AT(u32, 0x3D8) = (u32)(arg0 + 0x5C);

    if (Script_SeekToKeyword(0x50)) {
        IVY_AT(u16, 0x636) = SV(0);
        IVY_AT(u8, 0x63E) = SV(0);
        IVY_AT(u8, 0x63D) = SV(0);
    }
    IVY_AT(u32, 0x638) = Script_GetKeywordValue(0x64, 0);
    IVY_AT(u16, 0x5C) += 0x20;
    IVY_AT(u16, 0x60) += 0x20;
    if (IVY_AT(u8, 8) == 0) {
        *(u32 *)(IVY_AT(u8 *, 4) + 0x1C) = IVY_AT(u32, 0x5C);
        *(u32 *)(IVY_AT(u8 *, 4) + 0x20) = IVY_AT(u32, 0x60);
    } else {
        *(u32 *)(IVY_AT(u8 *, 4) + 0x40) = IVY_AT(u32, 0x5C);
        *(u32 *)(IVY_AT(u8 *, 4) + 0x44) = IVY_AT(u32, 0x60);
    }

    IVY_AT(u16, 0x1E6) = Script_GetKeywordValue(0x43, 0);
    IVY_AT(u16, 0x1E0) = 0x12A;
    IVY_AT(u16, 0x1E2) = 0x192;
    IVY_AT(u16, 0x1E4) = IVY_AT(u16, 0x1E6) + 0x12A;
    r1v = IVY_AT(u16, 0x1E0) + IVY_AT(u16, 0x1E6);
    if (IVY_AT(u8, 8) == 0)
        AnimData_SelectPaletteBank(IVY_AT(u8 *, 4) + 0x2C);
    else
        *(u8 **)(IVY_AT(u8 *, 4) + 0x68) = gPaletteBankData + (r1v << 5);
    if (IVY_AT(u8, 8) == 1)
        *(u16 *)(IVY_AT(u8 *, 4) + 0x5A) = SceneActorScript_GetManagerField62() - 0x5FFF;

    SceneActor_InitCollision5005(arg0);
    IVY_AT(u32, 0x2CC) |= 0x2000;
    IVY_AT(void *, 0x274) = Ivy_Update;
    IVY_AT(u32, 0x260) = 0;
    IVY_AT(u32, 0x224) = 0;
    IVY_AT(u32, 0x228) = 0;
    IVY_AT(u32, 0x22C) = 0;
    IVY_AT(u32, 0x230) = 0;
    IVY_AT(void *, 0x234) = Ivy_UpdateDefeat;
    IVY_AT(u32, 0x220) = 0;
    IVY_AT(void *, 0x278) = SceneActorReaction_OnEnemyDefault;
    IVY_AT(u32, 0x270) = 0;
    IVY_AT(const void *, 0x250) = gUnk_086108B0;
    IVY_AT(u32, 0x248) = 0;
    IVY_AT(u32, 0x26C) = 0;
    IVY_AT(void *, 0x238) = SceneActorReaction_SinkAndRemove;
    IVY_AT(void *, 0x264) = sub_081363F4;
    IVY_AT(void *, 0x268) = SceneActorControl_BeginSoundTransition;
    IVY_AT(u32, 0x240) = 0;
    IVY_AT(u32, 0x104) = 0;
    IVY_AT(u32, 0x108) = 0;
    IVY_AT(u32, 0x10C) = 0;
    IVY_AT(u32, 0x110) = 0;
    IVY_AT(u32, 0x114) = 0;
    IVY_AT(u32, 0x5D0) = 0;
    IVY_AT(u32, 0x5D4) = 0;
    IVY_AT(u32, 0x5F4) = 0;
    IVY_AT(u32, 0x5F8) = 0;
    IVY_AT(void *, 0x5FC) = SceneActorReaction_BeginFadedState;
    IVY_AT(void *, 0x600) = SceneActorReaction_BeginDefeatBurst;
    IVY_AT(u32, 0x43C) = 0;
    IVY_AT(u32, 0x440) = 0;
    IVY_AT(u8, 0x42B) = 1;
    IVY_AT(void *, 0x618) = sub_0807DB34;
    IVY_AT(u32, 0x608) = 0;
    IVY_AT(u32, 0x61C) = 0;
    IVY_AT(u32, 0x620) = 0;
    IVY_AT(void *, 0x5F0) = SceneActorReaction_RestoreAfterTurns;
    IVY_AT(void *, 0x60C) = sub_0807FCA8;
    IVY_AT(u32, 0x604) = 0;
    IVY_AT(void *, 0x614) = SceneActorReaction_TurnBackToRoute;
    IVY_AT(u32, 0x5E8) = 0;
    IVY_AT(void *, 0x444) = sub_0807F130;
    IVY_AT(void *, 0x41C) = SceneActorReaction_UpdateAnimationGate;
    IVY_AT(u16, 0x41A) = 0x28;
    IVY_AT(void *, 0x5DC) = sub_0808170C;
    IVY_AT(void *, 0x5E0) = SceneActorReaction_ResumeAfterAnimation;
    IVY_AT(void *, 0x5E4) = SceneActorReaction_TryResumeRoute;
    IVY_AT(void *, 0x610) = SceneActorReaction_BeginReactionSound;
    IVY_AT(void *, 0x248) = Ivy_TickDrawable;
    IVY_AT(const void *, 0x250) = gUnk_08606400;
    IVY_AT(u32, 0x2CC) |= 0x200;
    IVY_AT(u32, 0x2D0) |= 0x408;
    IVY_AT(u16, 0x2C0) |= 0xC0;
    if (Script_GetKeywordValue(0x45, 0))
        IVY_AT(u32, 0x2D0) |= 0x200000;
    SceneActorTerrain_ReadScriptParameters(arg0);
    flag = (IVY_AT(u16, 0x2C0) & 4) != 0;
    if (flag)
        IVY_AT(u32, 0x2D0) |= 0x2000;

    if (!flag) {
        IVY_AT(u8, 0x2A8) = 0;
        IVY_AT(u8, 0x2A9) = 0;
        IVY_AT(u8, 0x2AA) = 0;
        IVY_AT(u32, 0x2AC) = 0;
        IVY_AT(u8, 0x2B1) = 1;
        IVY_AT(u8, 0x2B2) = 1;
        IVY_AT(u8, 0x2B0) = 0;
        IVY_AT(u8, 0x2B5) = 0;
        IVY_AT(u32, 0x21C) = IVY_AT(u32, 0x22C);
        IVY_AT(u16, 0x2C8) = 0;
        IVY_AT(u32, 0x114) &= ~1;
        IVY_AT(u32, 0x104) = 0;
    }

    if (IVY_AT(u16, 0x2C0) & 2)
        IVY_AT(u16, 0x2FA) = *(u32 *)(gGameState + 0x5A0);
    else
        IVY_AT(u16, 0x2FA) = (u16)(u32)Region_FindContaining(arg0 + 0x5C);

    if (IVY_AT(u16, 0x2C0) & 0x40)
        IVY_AT(u16, 0x13A) &= 0xFEFF;

    if (Script_GetKeywordValue(0x53, 0) != 0 || *(u32 *)(gGameState + 0x5A0) != IVY_AT(u16, 0x2FA))
        SceneActorPlacement_HideActor(arg0, 0);

    SceneActorAnimation_SelectDirectionalEntry(arg0, 0);
    if (IVY_AT(u8, 8) == 0) {
        u8 *h = IVY_AT(u8 *, 4);
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
        Anim_Step(IVY_AT(u8 *, 4) + 0x20);
    }

    IVY_AT(u32, 0x304) = 0;
    SceneActorBehavior_InstallCallbackTables(arg0);
    Ivy_InitControlPresets(arg0);
    SceneActor_BeginReaction22(arg0);

    if (*(u32 *)(gGameState + 0x868) & 0x400) {
        IVY_AT(u8, 0x2A8) = 0;
        IVY_AT(u8, 0x2A9) = 0;
        IVY_AT(u8, 0x2AA) = 1;
        IVY_AT(u32, 0x2AC) = 0;
        IVY_AT(u8, 0x2B1) = 1;
        IVY_AT(u8, 0x2B2) = 1;
        IVY_AT(u8, 0x2B0) = 0;
        IVY_AT(u8, 0x2B5) = 0x2A;
        IVY_AT(void *, 0x21C) = SceneActorReaction_PlayHiddenCue;
        IVY_AT(u16, 0x2C8) = 0;
        IVY_AT(u32, 0x114) &= ~1;
        IVY_AT(u32, 0x104) = 0;
        IVY_AT(u16, 0x13A) = (IVY_AT(u16, 0x13A) | 8) & ~4;
        IVY_AT(u16, 0x2C0) &= ~0x800;
    }

    IVY_AT(u16, 0x12E) = IVY_AT(u16, 0x2C0);
    IVY_AT(u16, 0x12C) = IVY_AT(u16, 0x2EA);
    IVY_AT(u32, 0x118) = IVY_AT(u32, 0x2CC);
    IVY_AT(u32, 0x11C) = IVY_AT(u32, 0x2D0);
    IVY_AT(u8, 0x120) = *flagp;
    IVY_AT(u8, 0x121) = IVY_AT(u8, 0x2A8);
    IVY_AT(u8, 0x122) = IVY_AT(u8, 0x2A9);
    IVY_AT(u8, 0x123) = IVY_AT(u8, 0x2AA);
    IVY_AT(u32, 0x124) = IVY_AT(u32, 0x21C);
    IVY_AT(u8, 0x128) = IVY_AT(u8, 0x2B5);
    IVY_AT(void *, 0x24C) = SceneActorScript_OnCommand;
    IVY_AT(void *, 0x258) = Ivy_UpdateInteraction;
    IVY_AT(void *, 0x25C) = Ivy_Destroy;

    return SceneActorRegistry_LinkActor(arg0) ? 0 : -1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080F0C20);
#endif
