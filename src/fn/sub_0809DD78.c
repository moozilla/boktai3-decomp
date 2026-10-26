#include "global.h"
#include "render.h"

extern const u8 gUnk_08605E78[];

#define LUN_AT(T, off) (*(T *)((u8 *)arg0 + (off)))

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
void SceneActorControl_OnIdle(void);
void Lun_OnUpdate(void);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_ResolveDamage(void);
void SceneActorPlacement_PlayResponseAndResetState(void);
void SceneActorReaction_SinkAndRemove(void);
void SceneActorScript_OnCommand(void);
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
s32 Region_FindContaining(void *);

/* Script_GetPc() ? Script_GetValue() : dflt, inlined at every use */
static inline s32 Lun_0809DD78_Arg(s32 dflt)
{
    if (Script_GetPc())
        return Script_GetValue();
    return dflt;
}

void Lun_InitProfile(void *);
void ActorContact_InitializeBodyAndSecondary(void *);
void Lun_InitMotionPreset(void *);
void Lun_InitSecondaryActionTable(void *);
void Lun_InitReactionTable(void *);
void Path_GetPointPositionById(void *, s32, s32);
void SceneBody_StartRoute(void *, void *, s32, s32, s32);
s32 Route_GetCurrentPointFacing(void *);
void Lun_GetRouteProgressRate(void);
void Lun_TurnTowardSavedDirection(void);
void Lun_UpdateDirectionalPoseNoSelect(void);
void Lun_UpdateDirectionalPose(void);
void Lun_ApproachMovementTarget(void);
void Lun_UpdateDefeat(void);
void Lun_PlayResponseAndResetState(void);
void Lun_ResetMovementState(void);
void Lun_EnterIdleOrCopyCell(void);
void Lun_OnTargetCheck(void);
void Lun_OnPlayerTargetInit(void);
void Lun_Destroy(void);

#ifdef NONMATCHING
// fuzzy: 18.8%
// scope shape 1352 (differences once low registers are set aside)
// confidence=low: lun enemy setup: script parameters, animation tables, callback tables
s32 sub_0809DD78(void *arg0)
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
    u8 *route;
    u8 *p5c;
    u8 tmp3[8];
    s32 x, y;
    u8 *cellp;
    void *cell;

    zero = 0;
    LUN_AT(u32, 0x3D4) = 0;
    SceneActorRegistry_SetObjectContext(arg0, arg0, 0);
    LUN_AT(u8, 0x98) = 0x11;
    Lun_InitProfile(arg0);

    if (LUN_AT(u8, 8) == 0) {
        heap = Heap_Alloc(0x5C);
        LUN_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x5C);
        heap = LUN_AT(u8 *, 4);
        AnimData_Load(heap + 0x2C, 0xD28E);
        Sprite_Init(heap, heap + 0x2C, 0);
        SceneActorAnimation_InitEnemySequence(heap, 0x9607);
        if (LUN_AT(u8, 0x98) == 0x11 || LUN_AT(u8, 0x98) == 0x17)
            heap[7] = 1;
        LUN_AT(u8, 0x59) = 0;
        flagp = (u8 *)arg0 + 0x59;
    } else {
        heap = Heap_Alloc(0x80);
        LUN_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x80);
        heap = LUN_AT(u8 *, 4);
        CpuSet(&zero, tmp, 0x05000002);
        res = ScriptProc_Start(0xCB05, 0xD28E);
        for (i = 0; i < 8; i++)
            ((u32 *)heap)[i] = ((u32 *)res)[i];
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, tmp);
        LUN_AT(u8, 0x2B7) = 0xFF;
        flagp = (u8 *)arg0 + 0x59;
    }

    LUN_AT(u32, 0x2CC) = 0;
    LUN_AT(u32, 0x2D0) = 0;
    LUN_AT(u8, 0x2B4) = 0;
    LUN_AT(u16, 0x2E8) = 8;
    LUN_AT(u16, 0x2C0) = Script_GetKeywordValue(0x4D, 0);
    CpuSet(&zero, tmp2, 0x05000002);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        for (i = 0; i <= 3; i++) {
            LUN_AT(u16, 0xB4 + 2 * i) = Lun_0809DD78_Arg(0x18);
            LUN_AT(u16, 0xBC + 2 * i) = Lun_0809DD78_Arg(0x18);
            LUN_AT(u32, 0xCC + 4 * i) = Lun_0809DD78_Arg(0);
            LUN_AT(u16, 0xC4 + 2 * i) = Lun_0809DD78_Arg(0);
        }
    }

    if (Script_SeekToKeyword(0x65)) {
        r4v = Lun_0809DD78_Arg(0x30);
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
    LUN_AT(u32, 0xDC) = 0;
    LUN_AT(u16, 0xEC) = r4v;
    LUN_AT(u16, 0xEE) = r1v;
    LUN_AT(u8, 0xEA) = r2v;
    LUN_AT(u32, 0xF8) = r4v * r4v;
    LUN_AT(u32, 0xFC) = r1v * r1v;
    if (LUN_AT(u8, 0xEB) & 4)
        LUN_AT(u32, 0xF4) = LUN_AT(u32, 0xF8);
    else
        LUN_AT(u32, 0xF4) = Div(LUN_AT(u32, 0xF8) * 8 - LUN_AT(u32, 0xF8), 10);

    if (Script_SeekToKeyword(0x6C)) {
        r6v = Lun_0809DD78_Arg(0x12C);
        r4v = Lun_0809DD78_Arg(0x12C);
    } else {
        r6v = 0;
        r4v = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        LUN_AT(u32, 0x2D0) |= 0x200;
    LUN_AT(u32, 0x1F0) = (u16)Script_GetKeywordValue(0x46, 0);
    LUN_AT(u32, 0x1EC) = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        LUN_AT(u16, 0xB0) = Lun_0809DD78_Arg(0);
        LUN_AT(u8, 0x9E) = Lun_0809DD78_Arg(0);
        LUN_AT(u8, 0x9F) = Lun_0809DD78_Arg(0);
        LUN_AT(u8, 0xA0) = Lun_0809DD78_Arg(0);
        LUN_AT(u16, 0xA2) = Lun_0809DD78_Arg(0);
        LUN_AT(u16, 0xA4) = Lun_0809DD78_Arg(0);
    }

    r1v = LUN_AT(u16, 0xB0);
    r3v = r4v;
    LUN_AT(u16, 0x2D4) = 0;
    LUN_AT(void *, 0x21C) = SceneActorControl_OnIdle;
    LUN_AT(u8, 0x2B2) = 1;
    LUN_AT(u8, 0x2B0) = 0;
    if ((s16)r1v <= 0)
        r1v = 0;
    LUN_AT(u16, 0x9C) = r1v;
    LUN_AT(u16, 0xAA) = 0;
    if (r3v <= 0)
        r3v = r6v;
    LUN_AT(u16, 0xA6) = r6v;
    LUN_AT(u16, 0xA8) = r3v;
    LUN_AT(u16, 0x2FA) = 0;

    if (Script_SeekToKeyword(0x58)) {
        LUN_AT(u32, 0xAC) = Lun_0809DD78_Arg(0);
        LUN_AT(u8, 0xA1) = Lun_0809DD78_Arg(0);
    }
    LUN_AT(u16, 0xB2) = Script_GetKeywordValue(0x4B, 2);
    r1v = Script_GetKeywordValue(0x56, -1);
    if ((s16)r1v < 0)
        r1v = 0xFFFF;
    LUN_AT(u16, 0x9A) = r1v;
    if (Script_GetKeywordValue(0x48, 0))
        LUN_AT(u32, 0x2D0) |= 0x800;

    SceneBody_Init((u8 *)arg0 + 0x50, mode, tmp2, 0, 4, arg0);
    SceneBody_SetSprite((u8 *)arg0 + 0x50, LUN_AT(u8, 8) ? 0 : LUN_AT(u8 *, 4));
    SceneBody_SetMovementProbe((u8 *)arg0 + 0x50, (u8 *)arg0 + 0x40, 0x40, 0x40);

    route = (u8 *)arg0 + 0x42C;
    route[0xF] = 0;
    if (Script_SeekToKeyword(0x72)) {
        route[0x10] = Lun_0809DD78_Arg(0);
        route[0x11] = 0;
        route[0x12] = Lun_0809DD78_Arg(0);
        route[0xC] = route[0x12];
        route[0xD + route[0xF]] = route[0x10];
        route[0xD + (1 ^ route[0xF])] = Lun_0809DD78_Arg(route[0x10]);
        Path_GetPointPositionById(tmp3, route[0x10], route[0x12]);
        x = (s8)(*(u16 *)tmp3 >> 8);
        y = (s8)(*(u16 *)(tmp3 + 4) >> 8);
        if (x >= 0 && y >= 0 && (u32)x < *(u32 *)0x030052F8 && (u32)y < *(u32 *)0x030052FC)
            r1v = *(u16 *)(*(u8 **)0x030052F4 + 0x24 + 2 * y) + x;
        else
            r1v = 0;
        LUN_AT(u16, 0x2F4) = r1v;
        r1v = LUN_AT(u16, 0x2F4);
        cell = TerrainModifiers_FindCellOverride(r1v, 1);
        if (cell == 0)
            cellp = (u8 *)(*(u32 *)(*(u8 **)0x030052F4 + 4) + (r1v << 2) + 0xC);
        else
            cellp = (u8 *)cell + 4;
        r2v = *cellp >> 4;
        r1v = (*cellp & 0xF) << 8;
        if (r2v == 1)
            r1v -= tmp3[4];
        else if (r2v == 2)
            r1v -= tmp3[0];
        LUN_AT(u16, 0x2BE) = r1v;
        *(u16 *)(tmp3 + 2) = r1v;
        SceneBody_StartRoute((u8 *)arg0 + 0x50, route, route[0x10], route[0x11], route[0x12]);
        p5c = (u8 *)arg0 + 0x5C;
        *(u16 *)p5c = **(u16 **)(route + 8);
        *(u16 *)(p5c + 4) = (*(u16 **)(route + 8))[1];
        x = (s8)(*(u16 *)p5c >> 8);
        y = (s8)(*(u16 *)(p5c + 4) >> 8);
        if (x >= 0 && y >= 0 && (u32)x < *(u32 *)0x030052F8 && (u32)y < *(u32 *)0x030052FC)
            r1v = *(u16 *)(*(u8 **)0x030052F4 + 0x24 + 2 * y) + x;
        else
            r1v = 0;
        cell = TerrainModifiers_FindCellOverride(r1v, 1);
        if (cell == 0)
            cellp = (u8 *)(*(u32 *)(*(u8 **)0x030052F4 + 4) + (r1v << 2) + 0xC);
        else
            cellp = (u8 *)cell + 4;
        r2v = *cellp >> 4;
        r1v = (*cellp & 0xF) << 8;
        if (r2v == 1)
            r1v -= p5c[4];
        else if (r2v == 2)
            r1v -= p5c[0];
        *(u16 *)(p5c + 2) = r1v;
        if (LUN_AT(u8, 8) == 0) {
            u8 *h = LUN_AT(u8 *, 4);
            *(u32 *)(h + 0x1C) = LUN_AT(u32, 0x5C);
            *(u32 *)(h + 0x20) = LUN_AT(u32, 0x60);
        } else {
            u8 *h = LUN_AT(u8 *, 4);
            *(u32 *)(h + 0x40) = LUN_AT(u32, 0x5C);
            *(u32 *)(h + 0x44) = LUN_AT(u32, 0x60);
        }
        *flagp = Route_GetCurrentPointFacing(route);
    } else {
        route[0x10] = 0;
        route[0x11] = 0;
        route[0x12] = 0;
        route[0xC] = 0;
        LUN_AT(u8, 0x439 + route[0xF]) = 0;
        LUN_AT(u8, 0x439 + (1 ^ route[0xF])) = 0;
    }

    if (Script_SeekToKeyword(0x70)) {
        p5c = (u8 *)arg0 + 0x5C;
        *(u16 *)p5c = Lun_0809DD78_Arg(0);
        LUN_AT(u16, 0x5E) = Lun_0809DD78_Arg(0);
        LUN_AT(u16, 0x60) = Lun_0809DD78_Arg(0);
        x = (s8)(*(u16 *)p5c >> 8);
        y = (s8)(*(u16 *)(p5c + 4) >> 8);
        if (x >= 0 && y >= 0 && (u32)x < *(u32 *)0x030052F8 && (u32)y < *(u32 *)0x030052FC)
            r1v = *(u16 *)(*(u8 **)0x030052F4 + 0x24 + 2 * y) + x;
        else
            r1v = 0;
        LUN_AT(u16, 0x2F4) = r1v;
        r1v = LUN_AT(u16, 0x2F4);
        cell = TerrainModifiers_FindCellOverride(r1v, 1);
        if (cell == 0)
            cellp = (u8 *)(*(u32 *)(*(u8 **)0x030052F4 + 4) + (r1v << 2) + 0xC);
        else
            cellp = (u8 *)cell + 4;
        r2v = *cellp >> 4;
        r1v = (*cellp & 0xF) << 8;
        if (r2v == 1)
            r1v -= p5c[4];
        else if (r2v == 2)
            r1v -= p5c[0];
        LUN_AT(u16, 0x2BE) = r1v;
        LUN_AT(u16, 0x3DC) = r1v;
        if (LUN_AT(u8, 8) == 0) {
            u8 *h = LUN_AT(u8 *, 4);
            *(u32 *)(h + 0x1C) = LUN_AT(u32, 0x5C);
            *(u32 *)(h + 0x20) = LUN_AT(u32, 0x60);
        } else {
            u8 *h = LUN_AT(u8 *, 4);
            *(u32 *)(h + 0x40) = LUN_AT(u32, 0x5C);
            *(u32 *)(h + 0x44) = LUN_AT(u32, 0x60);
        }
    }

    if (Script_SeekToKeyword(0x50)) {
        LUN_AT(u8, 0x3D8) = Lun_0809DD78_Arg(0);
        LUN_AT(u8, 0x3D9) = Lun_0809DD78_Arg(0);
    }
    LUN_AT(u16, 0x442) = Script_GetKeywordValue(0x6C, 0);
    LUN_AT(u16, 0x1E6) = Script_GetKeywordValue(0x43, 0);
    LUN_AT(u16, 0x1E0) = 0x8C;
    LUN_AT(u16, 0x1E2) = 0x192;
    LUN_AT(u16, 0x1E4) = LUN_AT(u16, 0x1E6) + 0x91;
    r1v = LUN_AT(u16, 0x1E0) + LUN_AT(u16, 0x1E6);
    if (LUN_AT(u8, 8) == 0)
        AnimData_SelectPaletteBank(LUN_AT(u8 *, 4) + 0x2C);
    else
        *(u8 **)(LUN_AT(u8 *, 4) + 0x68) = gPaletteBankData + (r1v << 5);
    if (LUN_AT(u8, 8) == 1) {
        r1v = SceneActorScript_GetManagerField62();
        *(u16 *)(LUN_AT(u8 *, 4) + 0x20 + 0x3A) = r1v - 0x5FFF;
    }

    ActorContact_InitializeBodyAndSecondary(arg0);
    LUN_AT(u32, 0x2CC) |= 0x2000;
    LUN_AT(void *, 0x274) = Lun_OnUpdate;
    LUN_AT(void *, 0x260) = Lun_GetRouteProgressRate;
    LUN_AT(void *, 0x224) = Lun_TurnTowardSavedDirection;
    LUN_AT(void *, 0x228) = Lun_UpdateDirectionalPoseNoSelect;
    LUN_AT(void *, 0x22C) = Lun_UpdateDirectionalPose;
    LUN_AT(void *, 0x230) = Lun_ApproachMovementTarget;
    LUN_AT(void *, 0x234) = Lun_UpdateDefeat;
    LUN_AT(u32, 0x220) = 0;
    LUN_AT(void *, 0x278) = SceneActorReaction_OnEnemyDefault;
    LUN_AT(u32, 0x270) = 0;
    LUN_AT(u32, 0x248) = 0;
    LUN_AT(u32, 0x26C) = 0;
    LUN_AT(void *, 0x238) = SceneActorReaction_SinkAndRemove;
    LUN_AT(void *, 0x264) = SceneActorReaction_ResolveDamage;
    LUN_AT(void *, 0x268) = Lun_PlayResponseAndResetState;
    LUN_AT(u32, 0x240) = 0;
    LUN_AT(u32, 0x104) = 0;
    LUN_AT(u32, 0x108) = 0;
    LUN_AT(u32, 0x10C) = 0;
    LUN_AT(u32, 0x110) = 0;
    LUN_AT(u32, 0x114) = 0;
    LUN_AT(const u8 *, 0x250) = gUnk_08605E78;
    LUN_AT(void *, 0x248) = Lun_ResetMovementState;
    LUN_AT(void *, 0x23C) = Lun_EnterIdleOrCopyCell;
    LUN_AT(void *, 0x270) = Lun_OnTargetCheck;
    LUN_AT(u16, 0x2C0) |= 0x60;
    LUN_AT(u32, 0x2CC) |= 0x200;
    LUN_AT(u32, 0x2D0) |= 0x02100400;
    if (LUN_AT(u16, 0x1E6) != 0)
        LUN_AT(u32, 0x2D0) |= 8;
    SceneActorBehavior_InitializeShadow(arg0);
    SceneActorTerrain_ReadScriptParameters(arg0);
    flag = (LUN_AT(u16, 0x2C0) & 4) != 0;
    if (flag)
        LUN_AT(u32, 0x2D0) |= 0x2000;

    if (!flag) {
        LUN_AT(u8, 0x2A8) = 0;
        LUN_AT(u8, 0x2A9) = 0;
        LUN_AT(u8, 0x2AA) = 0;
        LUN_AT(u32, 0x2AC) = 0;
        LUN_AT(u8, 0x2B1) = 1;
        LUN_AT(u8, 0x2B2) = 1;
        LUN_AT(u8, 0x2B0) = 0;
        LUN_AT(u8, 0x2B5) = 0;
        LUN_AT(u32, 0x21C) = LUN_AT(u32, 0x22C);
        LUN_AT(u16, 0x2C8) = 0;
        LUN_AT(u32, 0x114) &= ~1;
        LUN_AT(u32, 0x104) = 0;
    }

    if (LUN_AT(u16, 0x2C0) & 2)
        LUN_AT(u16, 0x2FA) = *(u32 *)(*(u8 **)0x02000710 + 0x5A0);
    else
        LUN_AT(u16, 0x2FA) = Region_FindContaining((u8 *)arg0 + 0x5C);

    if (LUN_AT(u16, 0x2C0) & 0x40)
        LUN_AT(u16, 0x13A) &= 0xFEFF;

    if (Script_GetKeywordValue(0x53, 0) != 0 ||
        *(u32 *)(*(u8 **)0x02000710 + 0x5A0) != LUN_AT(u16, 0x2FA))
        SceneActorPlacement_HideActor(arg0, 0);

    SceneActorAnimation_SelectDirectionalEntry(arg0, 0);
    if (LUN_AT(u8, 8) != 0) {
        Anim_Step(LUN_AT(u8 *, 4) + 0x20);
    } else {
        u8 *h = LUN_AT(u8 *, 4);
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

    LUN_AT(u32, 0x304) = 0;
    SceneActorBehavior_InstallCallbackTables(arg0);
    Lun_InitMotionPreset(arg0);
    Lun_InitSecondaryActionTable(arg0);
    Lun_InitReactionTable(arg0);
    LUN_AT(u16, 0x2E8) = 2;
    LUN_AT(u16, 0x118 + 0x16) = LUN_AT(u16, 0x2C0);
    LUN_AT(u16, 0x118 + 0x14) = LUN_AT(u16, 0x2EA);
    LUN_AT(u32, 0x118) = LUN_AT(u32, 0x2CC);
    LUN_AT(u32, 0x118 + 4) = LUN_AT(u32, 0x2D0);
    LUN_AT(u8, 0x118 + 8) = *flagp;
    LUN_AT(u8, 0x118 + 9) = LUN_AT(u8, 0x2A8);
    LUN_AT(u8, 0x118 + 0xA) = LUN_AT(u8, 0x2A9);
    LUN_AT(u8, 0x118 + 0xB) = LUN_AT(u8, 0x2AA);
    LUN_AT(u32, 0x118 + 0xC) = LUN_AT(u32, 0x21C);
    LUN_AT(u8, 0x118 + 0x10) = LUN_AT(u8, 0x2B5);
    LUN_AT(void *, 0x24C) = SceneActorScript_OnCommand;
    LUN_AT(void *, 0x258) = Lun_OnPlayerTargetInit;
    LUN_AT(void *, 0x25C) = Lun_Destroy;

    return SceneActorRegistry_LinkActor(arg0) ? 0 : -1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0809DD78);
#endif
