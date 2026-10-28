#include "global.h"
#include "engine_globals.h"

void SceneActorBehavior_InitializeShadow(void *);

/* map cell index under a packed (x, y) position, or 0 when outside the map */

/* terrain height of a cell, adjusted by the slope bytes of pos */

// confidence=low: octopus enemy loader: script-driven parameters, animation, callback tables
#define OCT_AT(T, off) (*(T *)((u8 *)arg0 + (off)))

struct Oct32Block {
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
void Octopus_Update(void);
void Octopus_UpdateDefeat(void);
void SceneActorReaction_OnEnemyDefault(void);
void SceneActorReaction_SinkAndRemove(void);
void sub_081363F4(void);
void SceneActorControl_BeginSoundTransition_080EBC38(void);
void SceneActorControl_UpdateMotionPhase_080EBD0C(void);
void SceneActorReaction_BeginFadedState(void);
void SceneActorReaction_BeginDefeatBurst(void);
void SceneActorReaction_RestoreAfterTurns(void);
void SceneActorReaction_TurnBackToRoute(void);
void SceneActorReaction_UpdateAnimationGate(void);
void SceneActorReaction_ResumeAfterAnimation(void);
void SceneActorReaction_TryResumeRoute(void);
void SceneActorReaction_BeginReactionSound(void);
void sub_0807DB34(void);
void sub_0807FCA8(void);
void sub_0807F130(void);
void sub_0808170C(void);
void sub_08080BE0(void);
void SceneActorScript_OnCommand(void);
void Octopus_UpdateInteraction(void);
void Octopus_Destroy(void);
extern const u8 gUnk_086108B0[];
extern const u8 gUnk_0860635C[];
void Octopus_InitProfileId(void *);
void SceneActorCollision_InitCompactBody(void *);
void Octopus_InitControlPresets(void *);
void Octopus_BeginIdleAnimation(void *);
void Path_GetPointPositionById(void *, s32, s32);
void SceneBody_StartRoute(void *, void *, s32, s32, s32);
s32 Route_GetCurrentPointFacing(void *);
void TargetRegistry_RegisterActor(void *, void *, void *);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
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
void SceneActorTerrain_ReadScriptParameters(void *);
s32 SceneActorRegistry_LinkActor(void *);
void *Region_FindContaining(void *);

/* map cell index under a packed (x, y) position, or 0 when outside the map */
static inline s32 b_cell(u16 px, u16 py)
{
    s32 x = (s8)(px >> 8);
    s32 y = (s8)(py >> 8);

    if (x >= 0 && y >= 0 && (u32)x < gTerrainWidth && (u32)y < gTerrainHeight)
        return *(u16 *)(((u8 *)gTerrainMap) + 0x24 + y * 2) + x;
    return 0;
}

/* terrain height of a cell, adjusted by the slope bytes of pos */
static inline s32 b_height(s32 cell, u8 *pos)
{
    u8 *cp = TerrainModifiers_FindCellOverride(cell, 1);
    s32 h;
    s32 kind;

    if (cp != 0)
        cp += 4;
    else
        cp = *(u8 **)(((u8 *)gTerrainMap) + 4) + (cell << 2) + 0xC;
    kind = *cp >> 4;
    h = (*cp & 0xF) << 8;
    if (kind == 1)
        h -= pos[4];
    else if (kind == 2)
        h -= pos[0];
    return h;
}

#ifdef NONMATCHING
// fuzzy: 32.2%
// scope shape 1353 (differences once low registers are set aside)
// confidence=low: octopus enemy loader: script-driven parameters, animation, callback tables
s32 Octopus_Init(void *arg0)
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
    s32 r4v, r6v, r3v, r2v, r1v, s4, s6;
    u32 w0, w1;
    s32 x, y;
    u8 *cellp;
    void *cell;

    zero = 0;
    OCT_AT(u32, 0x62C) = 0;
    SceneActorRegistry_SetObjectContext(arg0, arg0, 1);
    OCT_AT(u8, 0x98) = 0xE;
    Octopus_InitProfileId(arg0);

    if (OCT_AT(u8, 8) == 0) {
        heap = Heap_Alloc(0x5C);
        OCT_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x5C);
        heap = OCT_AT(u8 *, 4);
        AnimData_Load(heap + 0x2C, 0xD291);
        Sprite_Init(heap, heap + 0x2C, 0);
        SceneActorAnimation_InitEnemySequence(heap, 0xDCEB);
        if (OCT_AT(u8, 0x98) == 0x11 || OCT_AT(u8, 0x98) == 0x17)
            heap[7] = 1;
        OCT_AT(u8, 0x59) = 0;
        flagp = (u8 *)arg0 + 0x59;
    } else {
        heap = Heap_Alloc(0x80);
        OCT_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x80);
        heap = OCT_AT(u8 *, 4);
        CpuSet(&zero, tmp, 0x05000002);
        res = ScriptProc_Start(0xCB05, 0xD291);
        *(struct Oct32Block *)heap = *(struct Oct32Block *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, tmp);
        OCT_AT(u8, 0x2B7) = 0xFF;
        flagp = (u8 *)arg0 + 0x59;
    }

    OCT_AT(u32, 0x2CC) = 0;
    OCT_AT(u32, 0x2D0) = 0;
    OCT_AT(u8, 0x2B4) = 0;
    OCT_AT(u16, 0x2E8) = 8;
    OCT_AT(u16, 0x2C0) = Script_GetKeywordValue(0x4D, 0);
    OCT_AT(u16, 0x2C2) = 0x40;
    CpuSet(&zero, tmp2, 0x05000002);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        for (i = 0; i <= 3; i++) {
            if (Script_GetPc())
                r1v = Script_GetValue();
            else
                r1v = 0x18;
            OCT_AT(u16, 0xB4 + 2 * i) = r1v;
            if (Script_GetPc())
                r2v = Script_GetValue();
            else
                r2v = 0x18;
            OCT_AT(u16, 0xBC + 2 * i) = r2v;
            if (Script_GetPc())
                r3v = Script_GetValue();
            else
                r3v = 0;
            OCT_AT(u32, 0xCC + 4 * i) = r3v;
            if (Script_GetPc())
                r4v = Script_GetValue();
            else
                r4v = 0;
            OCT_AT(u16, 0xC4 + 2 * i) = r4v;
        }
    }

    if (Script_SeekToKeyword(0x6C)) {
        r6v = Script_GetPc() ? Script_GetValue() : 0x12C;
        r4v = Script_GetPc() ? Script_GetValue() : 0x12C;
        r2v = Script_GetPc() ? Script_GetValue() : 0x12C;
    } else {
        r6v = 0;
        r4v = 0;
        r2v = 0;
    }
    OCT_AT(u16, 0x414) = 0;
    OCT_AT(u16, 0x416) = r2v;
    OCT_AT(u16, 0x418) = 0;
    OCT_AT(u16, 0x41A) = 0;
    OCT_AT(u32, 0x41C) = 0;
    if (Script_SeekToKeyword(0x6E)) {
        r3v = Script_GetPc() ? Script_GetValue() : 0;
        OCT_AT(u16, 0x420) = Script_GetPc() ? Script_GetValue() : 0;
    } else {
        r3v = 0;
        OCT_AT(u16, 0x420) = 0;
    }
    OCT_AT(u16, 0x422) = r3v;
    OCT_AT(u32, 0x424) = OCT_AT(u16, 0x422) * OCT_AT(u16, 0x422);
    OCT_AT(u8, 0x426) = 0;

    if (Script_SeekToKeyword(0x65)) {
        if (Script_GetPc())
            s4 = Script_GetValue();
        else
            s4 = 0x30;
        if (Script_GetPc())
            r2v = Script_GetValue() & 0xFF;
        else
            r2v = 0x40;
        s6 = s4;
    } else {
        s4 = 0x30;
        s6 = 0x30;
        r2v = 0x40;
    }
    OCT_AT(u32, 0xDC) = 0;
    OCT_AT(u16, 0xEC) = s4;
    OCT_AT(u16, 0xEE) = s6;
    OCT_AT(u8, 0xEA) = r2v;
    OCT_AT(u32, 0xF8) = s4 * s4;
    OCT_AT(u32, 0xFC) = s6 * s6;
    if (OCT_AT(u8, 0xEB) & 4)
        OCT_AT(u32, 0xF4) = OCT_AT(u32, 0xF8);
    else
        OCT_AT(u32, 0xF4) = Div(OCT_AT(u32, 0xF8) * 8 - OCT_AT(u32, 0xF8), 10);

    if (Script_GetKeywordValue(0x62, 0))
        OCT_AT(u32, 0x2D0) |= 0x200;
    OCT_AT(u32, 0x1F0) = (u16)Script_GetKeywordValue(0x46, 0);
    OCT_AT(u32, 0x1EC) = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        OCT_AT(u16, 0xB0) = Script_GetPc() ? Script_GetValue() : 0;
        OCT_AT(u8, 0x9E) = Script_GetPc() ? Script_GetValue() : 0;
        OCT_AT(u8, 0x9F) = Script_GetPc() ? Script_GetValue() : 0;
        OCT_AT(u8, 0xA0) = Script_GetPc() ? Script_GetValue() : 0;
        OCT_AT(u16, 0xA2) = Script_GetPc() ? Script_GetValue() : 0;
        OCT_AT(u16, 0xA4) = Script_GetPc() ? Script_GetValue() : 0;
    }

    r1v = OCT_AT(u16, 0xB0);
    r3v = r4v;
    OCT_AT(u16, 0x2D4) = 0;
    OCT_AT(void *, 0x21C) = SceneActorControl_OnIdle;
    OCT_AT(u8, 0x2B2) = 1;
    OCT_AT(u8, 0x2AC) = 0;
    if ((s16)r1v <= 0)
        r1v = 0;
    OCT_AT(u16, 0x9C) = r1v;
    OCT_AT(u16, 0xAA) = 0;
    if (r3v <= 0)
        r3v = r6v;
    OCT_AT(u16, 0xA6) = r6v;
    OCT_AT(u16, 0xA8) = r3v;
    OCT_AT(u16, 0x2FA) = 0;

    if (Script_SeekToKeyword(0x58)) {
        OCT_AT(u32, 0xAC) = Script_GetPc() ? Script_GetValue() : 0;
        OCT_AT(u8, 0xA1) = Script_GetPc() ? Script_GetValue() : 0;
    }
    OCT_AT(u16, 0xB2) = Script_GetKeywordValue(0x4B, 2);
    r1v = Script_GetKeywordValue(0x56, -1);
    if ((s16)r1v < 0)
        r1v = 0xFFFF;
    OCT_AT(u16, 0x9A) = r1v;
    if (Script_GetKeywordValue(0x48, 0))
        OCT_AT(u32, 0x2D0) |= 0x800;
    if (Script_GetKeywordValue(0x4C, 0))
        OCT_AT(u32, 0x2D0) |= 0x2000;

    SceneBody_Init(arg0 + 0x50, mode, tmp2, 0, 4, arg0);
    {
        u8 *rt = (u8 *)arg0 + 0x448;
        u8 *rp;
        u8 *p;
        u16 pbuf[4];

        rt[0xF] = 0;
        if (Script_SeekToKeyword(0x72)) {
            rt[0x10] = Script_GetPc() ? Script_GetValue() : 0;
            rt[0x11] = 0;
            rt[0x12] = Script_GetPc() ? Script_GetValue() : 0;
            rt[0xC] = rt[0x12];
            (rt + 0xD)[rt[0xF]] = rt[0x10];
            p = rt + 0xD + (1 ^ rt[0xF]);
            *p = Script_GetPc() ? Script_GetValue() : rt[0x10];
            Path_GetPointPositionById(pbuf, rt[0x10], rt[0x12]);
            r1v = b_cell(pbuf[0], pbuf[2]);
            OCT_AT(u16, 0x2F4) = r1v;
            OCT_AT(u16, 0x2BE) = b_height(OCT_AT(u16, 0x2F4), (u8 *)pbuf);
            pbuf[1] = OCT_AT(u16, 0x2BE);
            SceneBody_StartRoute(arg0 + 0x50, rt, rt[0x10], rt[0x11], rt[0x12]);
            rp = arg0 + 0x5C;
            *(u16 *)rp = *(u16 *)(*(u8 **)(rt + 8));
            *(u16 *)(rp + 4) = *(u16 *)(*(u8 **)(rt + 8) + 2);
            *(u16 *)(rp + 2) = b_height(b_cell(*(u16 *)rp, *(u16 *)(rp + 4)), rp);
            if (OCT_AT(u8, 8) == 0) {
                *(u32 *)(OCT_AT(u8 *, 4) + 0x1C) = OCT_AT(u32, 0x5C);
                *(u32 *)(OCT_AT(u8 *, 4) + 0x20) = OCT_AT(u32, 0x60);
            } else {
                *(u32 *)(OCT_AT(u8 *, 4) + 0x40) = OCT_AT(u32, 0x5C);
                *(u32 *)(OCT_AT(u8 *, 4) + 0x44) = OCT_AT(u32, 0x60);
            }
            OCT_AT(u8, 0x59) = Route_GetCurrentPointFacing(rt);
        } else {
            rt[0x10] = 0;
            rt[0x11] = 0;
            rt[0x12] = 0;
            rt[0xC] = 0;
            ((u8 *)arg0 + 0x3D4 + 0x81)[rt[0xF]] = 0;
            ((u8 *)arg0 + 0x3D4 + 0x81)[1 ^ rt[0xF]] = 0;
            rp = arg0 + 0x5C;
        }
        if (OCT_AT(u16, 0x2C0) & 0x200)
            TargetRegistry_RegisterActor(arg0 + 0x3D4 + 0x30, arg0 + 0x50, sub_08080BE0);
        SceneBody_SetSprite(arg0 + 0x50, OCT_AT(u8, 8) ? 0 : OCT_AT(u8 *, 4));
        if (OCT_AT(u8, 0x98) == 0x15)
            SceneBody_SetMovementProbe(arg0 + 0x50, arg0 + 0x40, 0x3C, 0x3C);
        else
            SceneBody_SetMovementProbe(arg0 + 0x50, arg0 + 0x40, 0x40, 0x40);
        *(u16 *)(arg0 + 0x3D4) = OCT_AT(u32, 0x50);
        *(u32 *)(arg0 + 0x3D4 + 4) = (u32)rp;
    }

    if (Script_SeekToKeyword(0x50)) {
        OCT_AT(u16, 0x636) = Script_GetPc() ? Script_GetValue() : 0;
        OCT_AT(u8, 0x63A) = Script_GetPc() ? Script_GetValue() : 0;
        OCT_AT(u8, 0x639) = Script_GetPc() ? Script_GetValue() : 0;
    }
    OCT_AT(u16, 0x5C) += 0x20;
    OCT_AT(u16, 0x60) += 0x20;
    if (OCT_AT(u8, 8) == 0) {
        *(u32 *)(OCT_AT(u8 *, 4) + 0x1C) = OCT_AT(u32, 0x5C);
        *(u32 *)(OCT_AT(u8 *, 4) + 0x20) = OCT_AT(u32, 0x60);
    } else {
        *(u32 *)(OCT_AT(u8 *, 4) + 0x40) = OCT_AT(u32, 0x5C);
        *(u32 *)(OCT_AT(u8 *, 4) + 0x44) = OCT_AT(u32, 0x60);
    }

    OCT_AT(u16, 0x1E6) = Script_GetKeywordValue(0x43, 0);
    OCT_AT(u16, 0x1E0) = 0x248;
    OCT_AT(u16, 0x1E2) = 0x192;
    OCT_AT(u16, 0x1E4) = OCT_AT(u16, 0x1E6) + 0x248;
    r1v = OCT_AT(u16, 0x1E0) + OCT_AT(u16, 0x1E6);
    if (OCT_AT(u8, 8) == 0)
        AnimData_SelectPaletteBank(OCT_AT(u8 *, 4) + 0x2C);
    else
        *(u32 *)(OCT_AT(u8 *, 4) + 0x68) = *(u32 *)0x030042E4 + (r1v << 5);
    if (OCT_AT(u8, 8) == 1) {
        r1v = SceneActorScript_GetManagerField62();
        OCT_AT(u16, 0x5A) = r1v - 0x5FFF;
    }

    SceneActorCollision_InitCompactBody(arg0);
    OCT_AT(u32, 0x2CC) |= 0x2000;
    OCT_AT(void *, 0x274) = Octopus_Update;
    OCT_AT(u32, 0x260) = 0;
    OCT_AT(u32, 0x224) = 0;
    OCT_AT(u32, 0x228) = 0;
    OCT_AT(u32, 0x22C) = 0;
    OCT_AT(u32, 0x230) = 0;
    OCT_AT(void *, 0x234) = Octopus_UpdateDefeat;
    OCT_AT(u32, 0x220) = 0;
    OCT_AT(void *, 0x278) = SceneActorReaction_OnEnemyDefault;
    OCT_AT(u32, 0x270) = 0;
    OCT_AT(void *, 0x250) = (void *)gUnk_086108B0;
    OCT_AT(u32, 0x248) = 0;
    OCT_AT(u32, 0x26C) = 0;
    OCT_AT(void *, 0x238) = SceneActorReaction_SinkAndRemove;
    OCT_AT(void *, 0x264) = sub_081363F4;
    OCT_AT(void *, 0x268) = SceneActorControl_BeginSoundTransition_080EBC38;
    OCT_AT(u32, 0x240) = 0;
    OCT_AT(u32, 0x104) = 0;
    OCT_AT(u32, 0x108) = 0;
    OCT_AT(u32, 0x10C) = 0;
    OCT_AT(u32, 0x110) = 0;
    OCT_AT(u32, 0x114) = 0;
    OCT_AT(u32, 0x5D0) = 0;
    OCT_AT(u32, 0x5D4) = 0;
    OCT_AT(u32, 0x5F4) = 0;
    OCT_AT(u32, 0x5F8) = 0;
    OCT_AT(void *, 0x5FC) = SceneActorReaction_BeginFadedState;
    OCT_AT(void *, 0x600) = SceneActorReaction_BeginDefeatBurst;
    OCT_AT(u32, 0x43C) = 0;
    OCT_AT(u32, 0x440) = 0;
    OCT_AT(void *, 0x618) = sub_0807DB34;
    OCT_AT(u32, 0x608) = 0;
    OCT_AT(u32, 0x61C) = 0;
    OCT_AT(u32, 0x620) = 0;
    OCT_AT(void *, 0x5F0) = SceneActorReaction_RestoreAfterTurns;
    OCT_AT(void *, 0x60C) = sub_0807FCA8;
    OCT_AT(u32, 0x604) = 0;
    OCT_AT(void *, 0x614) = SceneActorReaction_TurnBackToRoute;
    OCT_AT(u32, 0x5E8) = 0;
    OCT_AT(void *, 0x444) = sub_0807F130;
    OCT_AT(void *, 0x41C) = SceneActorReaction_UpdateAnimationGate;
    OCT_AT(u16, 0x41A) = 0x28;
    OCT_AT(void *, 0x5DC) = sub_0808170C;
    OCT_AT(void *, 0x5E0) = SceneActorReaction_ResumeAfterAnimation;
    OCT_AT(void *, 0x5E4) = SceneActorReaction_TryResumeRoute;
    OCT_AT(void *, 0x610) = SceneActorReaction_BeginReactionSound;
    OCT_AT(void *, 0x248) = SceneActorControl_UpdateMotionPhase_080EBD0C;
    OCT_AT(void *, 0x250) = (void *)gUnk_0860635C;
    OCT_AT(u32, 0x2CC) |= 0x200;
    OCT_AT(u32, 0x2D0) |= 0x204;
    OCT_AT(u16, 0x2C0) |= 0x40;
    if (Script_GetKeywordValue(0x45, 0))
        OCT_AT(u32, 0x2D0) |= 0x200000;
    SceneActorTerrain_ReadScriptParameters(arg0);
    flag = (OCT_AT(u16, 0x2C0) & 4) != 0;
    if (flag)
        OCT_AT(u32, 0x2D0) |= 0x2000;

    if (!flag) {
        OCT_AT(u8, 0x2A8) = 0;
        OCT_AT(u8, 0x2A9) = 0;
        OCT_AT(u8, 0x2AA) = 0;
        OCT_AT(u32, 0x2AC) = 0;
        OCT_AT(u8, 0x2B1) = 1;
        w0 = OCT_AT(u32, 0x22C);
        OCT_AT(u8, 0x2B2) = 1;
        OCT_AT(u8, 0x2B0) = 0;
        OCT_AT(u8, 0x2B5) = 0;
        OCT_AT(u32, 0x21C) = w0;
        OCT_AT(u16, 0x2C8) = 0;
        OCT_AT(u32, 0x114) &= ~1;
        OCT_AT(u32, 0x104) = 0;
    }

    if (OCT_AT(u16, 0x2C0) & 2)
        OCT_AT(u16, 0x2FA) = *(u32 *)(*(u8 **)0x02000710 + 0x5A0);
    else
        OCT_AT(u16, 0x2FA) = (u16)(u32)Region_FindContaining(arg0 + 0x5C);

    if (OCT_AT(u16, 0x2C0) & 0x40)
        OCT_AT(u16, 0x13A) &= 0xFEFF;

    if (Script_GetKeywordValue(0x53, 0) != 0 ||
        *(u32 *)(*(u8 **)0x02000710 + 0x5A0) != OCT_AT(u16, 0x2FA))
        SceneActorPlacement_HideActor(arg0, 0);

    SceneActorAnimation_SelectDirectionalEntry(arg0, 0);
    if (OCT_AT(u8, 8) != 0) {
        Anim_Step(OCT_AT(u8 *, 4) + 0x20);
    } else {
        u8 *h = OCT_AT(u8 *, 4);
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

    OCT_AT(u32, 0x304) = 0;
    SceneActorBehavior_InstallCallbackTables(arg0);
    Octopus_InitControlPresets(arg0);
    Octopus_BeginIdleAnimation(arg0);
    OCT_AT(u16, 0x12E) = OCT_AT(u16, 0x2C0);
    OCT_AT(u16, 0x12C) = OCT_AT(u16, 0x2EA);
    OCT_AT(u32, 0x118) = OCT_AT(u32, 0x2CC);
    OCT_AT(u32, 0x11C) = OCT_AT(u32, 0x2D0);
    OCT_AT(u8, 0x120) = *flagp;
    OCT_AT(u8, 0x121) = OCT_AT(u8, 0x2A8);
    OCT_AT(u8, 0x122) = OCT_AT(u8, 0x2A9);
    OCT_AT(u8, 0x123) = OCT_AT(u8, 0x2AA);
    OCT_AT(u32, 0x124) = OCT_AT(u32, 0x21C);
    OCT_AT(u8, 0x128) = OCT_AT(u8, 0x2B5);
    OCT_AT(void *, 0x24C) = SceneActorScript_OnCommand;
    OCT_AT(void *, 0x258) = Octopus_UpdateInteraction;
    OCT_AT(void *, 0x25C) = Octopus_Destroy;

    return SceneActorRegistry_LinkActor(arg0) ? 0 : -1;
}
#else
INCLUDE_ASM("asm/nonmatching", Octopus_Init);
#endif
