#include "global.h"
#include "render.h"

extern const u8 gUnk_086108B0[];

#define SLM_AT(T, off) (*(T *)((u8 *)arg0 + (off)))

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
static inline s32 Slime_08090FEC_Arg(s32 dflt)
{
    if (Script_GetPc())
        return Script_GetValue();
    return dflt;
}

void Slime_InitProfile(void *);
void Slime_InitColliders(void *);
void Slime_SetPrimaryStateTable(void *);
void Slime_SetSecondaryStateTable(void *);
void Bat_ReadScriptValuePair(void *);
void Slime_Update(void);
void Slime_GetAnimationMovePulse(void);
void Slime_ResetAnimation(void);
void Slime_RollMoveFlag(void);
void Slime_ConvergeReactionMotion(void);
void Bat_RunDefeatScript(void);
void Slime_ResolveDamageReaction(void);
void Slime_ResetIdlePlayback(void);
void Slime_FollowPlayer(void);
void Slime_TryEnrage(void);
void Slime_PlayPendingSound(void);
void Slime_OnMessage(void);
void Slime_Destroy(void);

#ifdef NONMATCHING
// fuzzy: 23.5%
// scope shape 1282 (differences once low registers are set aside)
// confidence=low: slime enemy setup: script parameters, animation tables, callback tables
s32 Slime_Init(void *arg0)
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
    s32 x, y;
    u8 *cellp;
    u8 *p5c;
    void *cell;

    zero = 0;
    SLM_AT(u16, 0x40A) = 0;
    SceneActorRegistry_SetObjectContext(arg0, arg0, 0);
    SLM_AT(u8, 0x98) = 0x16;
    Slime_InitProfile(arg0);

    if (SLM_AT(u8, 8) == 0) {
        heap = Heap_Alloc(0x5C);
        SLM_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x5C);
        heap = SLM_AT(u8 *, 4);
        AnimData_Load(heap + 0x2C, 0xB96C);
        Sprite_Init(heap, heap + 0x2C, 0);
        SceneActorAnimation_InitEnemySequence(heap, 0xA929);
        if (SLM_AT(u8, 0x98) == 0x11 || SLM_AT(u8, 0x98) == 0x17)
            heap[7] = 1;
        SLM_AT(u8, 0x59) = 0;
        flagp = (u8 *)arg0 + 0x59;
    } else {
        heap = Heap_Alloc(0x80);
        SLM_AT(u8 *, 4) = heap;
        Memory_Clear(heap, 0x80);
        heap = SLM_AT(u8 *, 4);
        CpuSet(&zero, tmp, 0x05000002);
        res = ScriptProc_Start(0xCB05, 0xB96C);
        for (i = 0; i < 8; i++)
            ((u32 *)heap)[i] = ((u32 *)res)[i];
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked(heap + 0x20, heap, 0, 0, 2, 2, 0x3C, tmp);
        SLM_AT(u8, 0x2B7) = 0xFF;
        flagp = (u8 *)arg0 + 0x59;
    }

    SLM_AT(u32, 0x2CC) = 0;
    SLM_AT(u32, 0x2D0) = 0;
    SLM_AT(u8, 0x2B4) = 0;
    SLM_AT(u16, 0x2E8) = 8;
    SLM_AT(u16, 0x2C0) = Script_GetKeywordValue(0x4D, 0);
    CpuSet(&zero, tmp2, 0x05000002);
    mode = Script_GetKeywordValue(0x4E, 0);

    if (Script_SeekToKeyword(0x61)) {
        for (i = 0; i <= 3; i++) {
            SLM_AT(u16, 0xB4 + 2 * i) = Slime_08090FEC_Arg(0x18);
            SLM_AT(u16, 0xBC + 2 * i) = Slime_08090FEC_Arg(0x18);
            SLM_AT(u32, 0xCC + 4 * i) = Slime_08090FEC_Arg(0);
            SLM_AT(u16, 0xC4 + 2 * i) = Slime_08090FEC_Arg(0);
        }
    }

    if (Script_SeekToKeyword(0x65)) {
        r4v = Slime_08090FEC_Arg(0x30);
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
    SLM_AT(u32, 0xDC) = 0;
    SLM_AT(u16, 0xEC) = r4v;
    SLM_AT(u16, 0xEE) = r1v;
    SLM_AT(u8, 0xEA) = r2v;
    SLM_AT(u32, 0xF8) = r4v * r4v;
    SLM_AT(u32, 0xFC) = r1v * r1v;
    if (SLM_AT(u8, 0xEB) & 4)
        SLM_AT(u32, 0xF4) = SLM_AT(u32, 0xF8);
    else
        SLM_AT(u32, 0xF4) = Div(SLM_AT(u32, 0xF8) * 8 - SLM_AT(u32, 0xF8), 10);

    if (Script_SeekToKeyword(0x6C)) {
        r6v = Slime_08090FEC_Arg(0x12C);
        r4v = Slime_08090FEC_Arg(0x12C);
    } else {
        r6v = 0;
        r4v = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        SLM_AT(u32, 0x2D0) |= 0x200;
    SLM_AT(u32, 0x1F0) = (u16)Script_GetKeywordValue(0x46, 0);
    SLM_AT(u32, 0x1EC) = (u16)Script_GetKeywordValue(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        SLM_AT(u16, 0xB0) = Slime_08090FEC_Arg(0);
        SLM_AT(u8, 0x9E) = Slime_08090FEC_Arg(0);
        SLM_AT(u8, 0x9F) = Slime_08090FEC_Arg(0);
        SLM_AT(u8, 0xA0) = Slime_08090FEC_Arg(0);
        SLM_AT(u16, 0xA2) = Slime_08090FEC_Arg(0);
        SLM_AT(u16, 0xA4) = Slime_08090FEC_Arg(0);
    }

    r1v = SLM_AT(u16, 0xB0);
    r3v = r4v;
    SLM_AT(u16, 0x2D4) = 0;
    SLM_AT(void *, 0x21C) = SceneActorControl_OnIdle;
    SLM_AT(u8, 0x2B2) = 1;
    SLM_AT(u8, 0x2B0) = 0;
    if ((s16)r1v <= 0)
        r1v = 0;
    SLM_AT(u16, 0x9C) = r1v;
    SLM_AT(u16, 0xAA) = 0;
    if (r3v <= 0)
        r3v = r6v;
    SLM_AT(u16, 0xA6) = r6v;
    SLM_AT(u16, 0xA8) = r3v;
    SLM_AT(u16, 0x2FA) = 0;

    if (Script_SeekToKeyword(0x58)) {
        SLM_AT(u32, 0xAC) = Slime_08090FEC_Arg(0);
        SLM_AT(u8, 0xA1) = Slime_08090FEC_Arg(0);
    }
    SLM_AT(u16, 0xB2) = Script_GetKeywordValue(0x4B, 2);
    r1v = Script_GetKeywordValue(0x56, -1);
    if ((s16)r1v < 0)
        r1v = 0xFFFF;
    SLM_AT(u16, 0x9A) = r1v;
    if (Script_GetKeywordValue(0x48, 0))
        SLM_AT(u32, 0x2D0) |= 0x800;

    SceneBody_Init((u8 *)arg0 + 0x50, mode, tmp2, 0, 4, arg0);
    SceneBody_SetSprite((u8 *)arg0 + 0x50, SLM_AT(u8, 8) ? 0 : SLM_AT(u8 *, 4));
    SceneBody_SetMovementProbe((u8 *)arg0 + 0x50, (u8 *)arg0 + 0x40, 0x40, 0x40);
    SLM_AT(u16, 0x3DA) = SLM_AT(s16, 0x9C) >> 1;

    if (Script_SeekToKeyword(0x70)) {
        p5c = (u8 *)arg0 + 0x5C;
        *(u16 *)p5c = Slime_08090FEC_Arg(0);
        SLM_AT(u16, 0x5E) = Slime_08090FEC_Arg(0);
        SLM_AT(u16, 0x60) = Slime_08090FEC_Arg(0);
        x = (s8)(*(u16 *)p5c >> 8);
        y = (s8)(*(u16 *)(p5c + 4) >> 8);
        if (x >= 0 && y >= 0 && (u32)x < *(u32 *)0x030052F8 && (u32)y < *(u32 *)0x030052FC)
            r1v = *(u16 *)(*(u8 **)0x030052F4 + 0x24 + 2 * y) + x;
        else
            r1v = 0;
        SLM_AT(u16, 0x2F4) = r1v;
        r1v = SLM_AT(u16, 0x2F4);
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
        SLM_AT(u16, 0x2BE) = r1v;
        SLM_AT(u16, 0x3D8) = r1v;
        if (SLM_AT(u8, 8) == 0) {
            u8 *h = SLM_AT(u8 *, 4);
            *(u32 *)(h + 0x1C) = SLM_AT(u32, 0x5C);
            *(u32 *)(h + 0x20) = SLM_AT(u32, 0x60);
        } else {
            u8 *h = SLM_AT(u8 *, 4);
            *(u32 *)(h + 0x40) = SLM_AT(u32, 0x5C);
            *(u32 *)(h + 0x44) = SLM_AT(u32, 0x60);
        }
    }

    r1v = Script_SeekToKeyword(0x6A);
    if (r1v) {
        SLM_AT(u16, 0x2EC) = Slime_08090FEC_Arg(0);
        SLM_AT(u16, 0x2EE) = Slime_08090FEC_Arg(0);
        SLM_AT(u16, 0x2F0) = Slime_08090FEC_Arg(0);
        SLM_AT(u16, 0x40A) |= 0x18;
        r1v = Script_GetKeywordValue(0x6D, 0);
        if (r1v)
            SLM_AT(u16, 0xB0) = r1v;
        SLM_AT(u16, 0x2E8) = 1;
    } else {
        SLM_AT(u16, 0x2E8) = r1v;
    }

    SLM_AT(u16, 0x178) = Script_GetKeywordValue(0x6E, 0);
    SLM_AT(u16, 0x3D6) = 0x258;
    SLM_AT(u16, 0x2EA) = Script_GetKeywordValue(0x66, 0);
    if (SLM_AT(u16, 0x2EA))
        SLM_AT(u16, 0x40A) |= 0x10;
    Bat_ReadScriptValuePair(arg0);

    SLM_AT(u16, 0x1E6) = Script_GetKeywordValue(0x43, 0);
    SLM_AT(u16, 0x1E0) = 0xCC;
    SLM_AT(u16, 0x1E2) = 0x192;
    SLM_AT(u16, 0x1E4) = SLM_AT(u16, 0x1E6) + 0xCC;
    r1v = SLM_AT(u16, 0x1E0) + SLM_AT(u16, 0x1E6);
    if (SLM_AT(u8, 8) == 0)
        AnimData_SelectPaletteBank(SLM_AT(u8 *, 4) + 0x2C);
    else
        *(u8 **)(SLM_AT(u8 *, 4) + 0x68) = gPaletteBankData + (r1v << 5);
    if (SLM_AT(u8, 8) == 1) {
        r1v = SceneActorScript_GetManagerField62();
        *(u16 *)(SLM_AT(u8 *, 4) + 0x20 + 0x3A) = r1v - 0x5FFF;
    }

    Slime_InitColliders(arg0);
    SLM_AT(u32, 0x2CC) |= 0x2000;
    SLM_AT(void *, 0x274) = Slime_Update;
    SLM_AT(void *, 0x260) = Slime_GetAnimationMovePulse;
    SLM_AT(u32, 0x224) = 0;
    SLM_AT(void *, 0x228) = Slime_ResetAnimation;
    SLM_AT(void *, 0x22C) = Slime_RollMoveFlag;
    SLM_AT(void *, 0x230) = Slime_ConvergeReactionMotion;
    SLM_AT(void *, 0x234) = Bat_RunDefeatScript;
    SLM_AT(u32, 0x220) = 0;
    SLM_AT(void *, 0x278) = SceneActorReaction_OnEnemyDefault;
    SLM_AT(u32, 0x270) = 0;
    SLM_AT(const u8 *, 0x250) = gUnk_086108B0;
    SLM_AT(u32, 0x248) = 0;
    SLM_AT(u32, 0x26C) = 0;
    SLM_AT(void *, 0x238) = SceneActorReaction_SinkAndRemove;
    SLM_AT(void *, 0x264) = SceneActorReaction_ResolveDamage;
    SLM_AT(void *, 0x268) = Slime_ResolveDamageReaction;
    SLM_AT(u32, 0x240) = 0;
    SLM_AT(u32, 0x104) = 0;
    SLM_AT(u32, 0x108) = 0;
    SLM_AT(u32, 0x10C) = 0;
    SLM_AT(u32, 0x110) = 0;
    SLM_AT(u32, 0x114) = 0;
    SLM_AT(void *, 0x248) = Slime_ResetIdlePlayback;
    SLM_AT(void *, 0x23C) = Slime_FollowPlayer;
    SLM_AT(void *, 0x270) = Slime_TryEnrage;
    SLM_AT(void *, 0x240) = Slime_PlayPendingSound;
    SLM_AT(u32, 0x2D0) |= 8;
    SLM_AT(u16, 0x2C0) |= 0x40;
    SceneActorBehavior_InitializeShadow(arg0);
    SceneActorTerrain_ReadScriptParameters(arg0);
    flag = (SLM_AT(u16, 0x2C0) & 4) != 0;
    if (flag)
        SLM_AT(u32, 0x2D0) |= 0x2000;

    if (!flag) {
        SLM_AT(u8, 0x2A8) = 0;
        SLM_AT(u8, 0x2A9) = 0;
        SLM_AT(u8, 0x2AA) = 0;
        SLM_AT(u32, 0x2AC) = 0;
        SLM_AT(u8, 0x2B1) = 1;
        SLM_AT(u8, 0x2B2) = 1;
        SLM_AT(u8, 0x2B0) = 0;
        SLM_AT(u8, 0x2B5) = 0;
        SLM_AT(u32, 0x21C) = SLM_AT(u32, 0x22C);
        SLM_AT(u16, 0x2C8) = 0;
        SLM_AT(u32, 0x114) &= ~1;
        SLM_AT(u32, 0x104) = 0;
    }

    if (SLM_AT(u16, 0x2C0) & 2)
        SLM_AT(u16, 0x2FA) = *(u32 *)(*(u8 **)0x02000710 + 0x5A0);
    else
        SLM_AT(u16, 0x2FA) = Region_FindContaining((u8 *)arg0 + 0x5C);

    if (SLM_AT(u16, 0x2C0) & 0x40)
        SLM_AT(u16, 0x13A) &= 0xFEFF;

    if (Script_GetKeywordValue(0x53, 0) != 0 ||
        *(u32 *)(*(u8 **)0x02000710 + 0x5A0) != SLM_AT(u16, 0x2FA))
        SceneActorPlacement_HideActor(arg0, 0);

    SceneActorAnimation_SelectDirectionalEntry(arg0, 0);
    if (SLM_AT(u8, 8) != 0) {
        Anim_Step(SLM_AT(u8 *, 4) + 0x20);
    } else {
        u8 *h = SLM_AT(u8 *, 4);
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

    SLM_AT(u32, 0x304) = 0;
    if (SLM_AT(u16, 0x2C0) & 8) {
        SLM_AT(u8, 0x2B2) = 1;
        SLM_AT(u8, 0x2B0) = 0;
        SLM_AT(u8, 0x2B5) = 0x10;
        SLM_AT(void *, 0x21C) = SceneActorControl_OnIdle;
        SLM_AT(u16, 0x2C8) = 0;
        SLM_AT(u32, 0x114) &= ~1;
        SLM_AT(u32, 0x104) = 0;
        SLM_AT(u8, 0x2A8) = 0;
        SLM_AT(u8, 0x2A9) = 0;
        SLM_AT(u8, 0x2AA) = 2;
        SLM_AT(u32, 0x2AC) = 0;
        SLM_AT(u8, 0x2B1) = 1;
        SLM_AT(u32, 0x2D0) |= 0x800;
    }

    SceneActorBehavior_InstallCallbackTables(arg0);
    Slime_SetPrimaryStateTable(arg0);
    Slime_SetSecondaryStateTable(arg0);
    SLM_AT(u16, 0x118 + 0x16) = SLM_AT(u16, 0x2C0);
    SLM_AT(u16, 0x118 + 0x14) = SLM_AT(u16, 0x2EA);
    SLM_AT(u32, 0x118) = SLM_AT(u32, 0x2CC);
    SLM_AT(u32, 0x118 + 4) = SLM_AT(u32, 0x2D0);
    SLM_AT(u8, 0x118 + 8) = *flagp;
    SLM_AT(u8, 0x118 + 9) = SLM_AT(u8, 0x2A8);
    SLM_AT(u8, 0x118 + 0xA) = SLM_AT(u8, 0x2A9);
    SLM_AT(u8, 0x118 + 0xB) = SLM_AT(u8, 0x2AA);
    SLM_AT(u32, 0x118 + 0xC) = SLM_AT(u32, 0x21C);
    SLM_AT(u8, 0x118 + 0x10) = SLM_AT(u8, 0x2B5);
    SLM_AT(void *, 0x24C) = SceneActorScript_OnCommand;
    SLM_AT(void *, 0x258) = Slime_OnMessage;
    SLM_AT(void *, 0x25C) = Slime_Destroy;

    return SceneActorRegistry_LinkActor(arg0) ? 0 : -1;
}
#else
INCLUDE_ASM("asm/nonmatching", Slime_Init);
#endif
