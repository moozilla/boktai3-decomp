#include "global.h"
#include "render.h"
// MATCHING_CONCESSION: register-pinning

#ifdef NONMATCHING
// fuzzy: 83.7%
// Remaining: constant-address reuse, PRE stack order, palette registers and state tails.
struct VanargandPart {
    u8 pad0[0x10];
    u16 state;
};
struct Vanargand {
    u8 pad0[0x54];
    u8 motionData[0x90 - 0x54];
    u32 actionTimer;
    s16 health;
    u8 pad96[6];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[0xAF - 0xA8];
    u8 finished, entered, pendingAnimation, ready;
    u8 padB3[0x18C - 0xB3];
    u8 animation[0x60];
    u8 drawable[0x3FC - 0x1EC];
    struct VanargandPart part;
    u8 pad410[0x500 - 0x410];
    u16 partActive;
    u8 pad502[0x534 - 0x502];
    u8 track[0x29];
    u8 trackState;
    u8 pad55E[0x56C - 0x55E];
    void *attachment;
    u8 pad570[0x584 - 0x570];
    u16 pairedMotion;
    u8 pad586[0x59E - 0x586];
    u16 thresholds[3];
    u32 hitFlags;
    s16 damage;
    u8 pad5AA[2];
    s16 accumulatedDamage;
    u16 flashTimer;
    u16 delay;
    u8 pad5B2[0x5B8 - 0x5B2];
    u16 field5B8, field5BA;
    u8 pad5BC[0x718 - 0x5BC];
    u8 palette[0x918 - 0x718];
    u8 fadeA[0xB18 - 0x918];
    u8 fadeB[0xD18 - 0xB18];
    u8 fadeC[0xF24 - 0xD18];
    u32 variant;
    u8 padF28[0xF34 - 0xF28];
    s32 transition;
    u8 padF38[0xF44 - 0xF38];
    void *afterimages;
};
s32 __divsi3(s32, s32);
void Sound_PlaySong(u32);
void Vanargand_ResetPaletteTransition(struct Vanargand *);
void CpuSet(const void *, void *, u32);
extern u16 gUnk_03004FC0[];

#define LERP(dst, t, from, to, tag)                                                                \
    {                                                                                              \
        if ((t) > 0)                                                                               \
            goto tag##Upper;                                                                       \
        dst = (from);                                                                              \
        goto tag##Store;                                                                           \
        tag##Upper:                                                                                \
        {                                                                                          \
            s32 limit = 3;                                                                         \
            if ((t) < limit)                                                                       \
                goto tag##Interp;                                                                  \
        }                                                                                          \
        dst = (to);                                                                                \
        goto tag##Store;                                                                           \
        tag##Interp:                                                                               \
        {                                                                                          \
            s32 k = ((to) - (from)) << 8;                                                          \
            s32 v = (t) * k;                                                                       \
            s32 d = 3;                                                                             \
            s32 result;                                                                            \
            v = v / d;                                                                             \
            if (v >= 0)                                                                            \
                result = v >> 8;                                                                   \
            else                                                                                   \
                result = -(-v >> 8);                                                               \
            dst = result;                                                                          \
            dst += (from);                                                                         \
        }                                                                                          \
        tag##Store :;                                                                              \
    }

#define FIELD(type, p, offset) (*(type *)((u8 *)(p) + (offset)))
void Vanargand_StopActionSequence(void *);
static inline void restorePalette(struct Vanargand *p, struct Vanargand *self)
{
    s32 kind = 0x6F;
    if (!p->variant)
        kind = 0x7F;
    switch (kind) {
        case 0x6F: {
            s32 t;
            s32 r, g, b;

            CpuSet(gPaletteBankData + 0xDE0, p->palette, 0x04000020);
            CpuSet(p->fadeA, gUnk_03004FC0, 0x04000068);
            t = p->transition;
            LERP(r, t, 7, 0x13, r1)
            LERP(g, t, 0, 0x12, g1)
            LERP(b, t, 0, 0x13, b1)
            r |= g << 5;
            r |= b << 10;
            {
                u16 color = r;
                u16 *pal = gUnk_03004FC0;
                pal[0x56 / 2] = color;
                pal[0x10C / 2] = color;
            }
            LERP(r, t, 0x15, 0x18, r2)
            LERP(g, t, 0, 0x1D, g2)
            LERP(b, t, 0, 0x1F, b2)
            r |= g << 5;
            r |= b << 10;
            {
                u16 *pal;
                u32 color = (u16)r;
                // Complete packing before materializing the second palette address.
                asm("" : : "r"(color));
                pal = gUnk_03004FC0;
                pal[0x5A / 2] = color;
                pal[0x10E / 2] = color;
            }
            break;
        }
        case 0x82:
            CpuSet(gPaletteBankData + 0x1040, p->palette, 0x04000020);
            CpuSet(p->fadeB, gUnk_03004FC0, 0x04000068);
            break;
        case 0x7F:
            CpuSet(gPaletteBankData + 0xFE0, p->palette, 0x04000018);
            CpuSet(p->fadeC, gUnk_03004FC0, 0x04000068);
            break;
        default:
            CpuSet(gPaletteBankData + 0x3240, self->palette, 0x04000008);
            break;
    }
    Vanargand_ResetPaletteTransition(self);
}
static inline void changeState(struct Vanargand *p, s32 state, void *callback)
{
    struct Vanargand *self = p;
    // Keep the transition owner distinct from the callback installation owner.
    if (FIELD(u8, p, 0xAB) == 3) {
        restorePalette(p, self);
        Vanargand_StopActionSequence((u8 *)self + 0x21C);
    }
    {
        u32 zero = 0;
        FIELD(u8, p, 0xAB) = state;
        FIELD(void *, p, 0x100) = callback;
        FIELD(u32, p, 0x90) = zero;
        p->pendingAnimation = 1;
        p->phase = zero;
        p->timer = zero;
    }
}

extern u8 *gUnk_02000580[];
s32 Script_GetKeywordValue(u32, s32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void SceneActors_Register(void *, s32);
void SceneActors_SetEventScripts(void *, s32, s32);
void SceneActors_Initialize(void *, void *, s32, void *, s32);
u8 *ScriptProc_Start(u32, u32);
void SceneBackdrop_Initialize(void *, void *);
void Vanargand_InitAnimatedAttachment(void *, void *);
void Vanargand_InitSpriteGroup(void *, void *, void *, void *);
void Vanargand_InitPart(void *, void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void SceneActors_SetHealthLimits(void *, s32, s32, s32);
void PositionedEffect_InitAttackCollider(void *);
void PositionedEffect_InitRadialAttackColliders(void *);
void PositionedEffect_InitContactCollider(void *);
void ActorSoundPhase_StartFlaggedAnimation(void);
void Vanargand_OnPrimaryStopState(void);
void Vanargand_OnAlternateStopState(void);
void *Vanargand_CreatePartController(void *, void *, s32);
void PairedSceneActor_InitHalves(void *, s32, s32, s32, s32, s32, s32, s32, s32);
void Vanargand_SetPartAnimationParameter(void *, s32);
void PairedSceneActor_SetResetValues(void *, s32, s32);
void Vanargand_SetPairedEffectParameter(void *, s32);
void Vanargand_ApplyPendingDamage(void);
void Vanargand_ApplyRequestedMode(void);
void *VanargandProjectilePool_Create(s32, s32);
void *VanargandAfterimagePool_Create(void);
struct InitPosition {
    s32 x : 16, y : 16, z : 16;
};
static inline void setMode(struct Vanargand *p, s32 mode, s32 step, s32 phase)
{
    FIELD(u8, p, 0xAC) = mode;
    FIELD(u8, p, 0xAD) = step;
    FIELD(u8, p, 0xAE) = phase;
    FIELD(u32, p, 0xC4) = 0;
    FIELD(u8, p, 0xB0) = 1;
}
static inline void placeBody(struct Vanargand *p)
{
    s16 *pos = (void *)((u8 *)p + 0x54);
    s32 xz = 0x33B;
    s32 y = 0x100;
    pos[0] = xz;
    pos[1] = y;
    pos[2] = xz;
}
// confidence=low: initializes the script-selected Vanargand components and starting mode.
#define Vanargand_Init sub_081AE0C0
s32 Vanargand_Init(struct Vanargand *p, void *arg)
{
    struct Vanargand *self = p;
    struct InitPosition position;
    s32 type = Script_GetKeywordValue('t', 5);
    s32 scale;
    register s32 health asm("sl");
    s32 limitA, limitB;
    s32 effect;
    s32 mode;
    s32 flags;
    s32 i;
    if (type != 5)
        return -1;
    SceneActors_Register(p, 5);
    FIELD(u32, p, 0x570) = 1;
    asm("" : : "r"(&p->variant), "r"(p->palette));
    {
        u8 **target = (void *)((u8 *)p + 0x110);
        u8 **slot = (void *)((u8 *)p + 0x10C);
        u8 **player = gUnk_02000580;
        s32 count = 1;
        do {
            u8 *value = *player;
            if (value) {
                *slot = value;
                value += 0x30;
            } else
                *slot = value;
            *target = value;
            target++;
            slot++;
            player++;
        } while (--count != 0);
    }
    if (Script_SeekToKeyword('s')) {
        Script_GetValue();
        scale = Script_GetValue();
    } else
        scale = 100;
    if (Script_SeekToKeyword('a')) {
        FIELD(u16, p, 0x584) = Script_GetValue();
        FIELD(u16, p, 0x586) = Script_GetValue();
        FIELD(u16, p, 0x588) = Script_GetValue();
        FIELD(u16, p, 0x58A) = Script_GetValue();
        FIELD(u16, p, 0x58C) = Script_GetValue();
        FIELD(u16, p, 0x58E) = Script_GetValue();
    } else {
        FIELD(u16, p, 0x584) = 1;
        FIELD(u16, p, 0x586) = 2;
        FIELD(u16, p, 0x588) = 4;
        FIELD(u16, p, 0x58A) = 8;
        FIELD(u16, p, 0x58C) = 16;
        FIELD(u16, p, 0x58E) = 32;
    }
    if (Script_SeekToKeyword('L')) {
        health = Script_GetValue();
        limitA = Script_GetValue();
        limitB = Script_GetValue();
    } else {
        health = 1000;
        limitA = limitB = 300;
    }
    if (Script_SeekToKeyword('S')) {
        u16 *out = (void *)((u8 *)p + 0x598);
        for (i = 2; i >= 0; i--)
            *out++ = Script_GetValue();
    } else {
        s32 value = 200;
        for (i = 2; i >= 0; i--)
            FIELD(u16, p, 0x598 + i * 2) = value;
    }
    if (Script_SeekToKeyword('V')) {
        u16 *out = (void *)((u8 *)p + 0x59E);
        for (i = 2; i >= 0; i--)
            *out++ = Script_GetValue();
    } else {
        s32 value = 200;
        for (i = 2; i >= 0; i--)
            FIELD(u16, p, 0x59E + i * 2) = value;
    }
    if (Script_SeekToKeyword('W')) {
        Script_GetValue();
        FIELD(u16, p, 0x51E) = Script_GetValue();
        effect = Script_GetValue();
    } else {
        FIELD(u16, p, 0x51E) = 0;
        effect = 0;
    }
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
        flags = 32;
        asm("" : "+r"(flags));
    } else {
        position.x = 640;
        position.y = 768;
        position.z = 640;
        flags = 32;
    }
    if (Script_SeekToKeyword('m'))
        p->variant = Script_GetValue();
    else
        p->variant = 1;
    FIELD(u32, p, 0xF38) = Script_GetKeywordValue('E', 0);
    mode = Script_GetKeywordValue('D', 0);
    if (Script_SeekToKeyword('e')) {
        u32 *out = (void *)((u8 *)p + 0xF28);
        for (i = 2; i >= 0; i--)
            *out++ = Script_GetValue();
    } else {
        s32 value = 0;
        for (i = 2; i >= 0; i--)
            FIELD(u32, p, 0xF28 + i * 4) = value;
    }
    if (Script_SeekToKeyword('c')) {
        s32 a = Script_GetValue();
        s32 b = Script_GetValue();
        SceneActors_SetEventScripts(self, a, b);
    } else
        SceneActors_SetEventScripts(self, 0, 0);
    SceneActors_Initialize(self, arg, type, &position, flags);
    CpuSet(gPaletteBankData + 0xDE0, p->palette, 0x04000080);
    CpuSet(ScriptProc_Start(0x92B3, 0x454B) + 0x14, p->fadeA, 0x04000080);
    CpuSet(ScriptProc_Start(0x92B3, 0xEEE9) + 0x14, p->fadeB, 0x04000080);
    CpuSet(ScriptProc_Start(0x92B3, 0xF392) + 0x14, p->fadeC, 0x04000080);
    SceneBackdrop_Initialize(p, (u8 *)p + 0x174);
    Vanargand_InitAnimatedAttachment(p, (u8 *)p + 0x18C);
    Vanargand_InitSpriteGroup(p, (u8 *)p + 0x2D0, (u8 *)p + 0x184, (u8 *)p + 0x214);
    if (p->variant)
        Vanargand_InitPart(p, (u8 *)p + 0x3FC);
    else {
        s16 *pos = (void *)((u8 *)p + 0x418);
        pos[0] = 0x33B;
        pos[1] = 0x100;
        pos[2] = 0x33B;
    }
    restorePalette(p, p);
    SceneActors_StartUnmirroredAnimation(self, (u8 *)p + 0x18C, (u8 *)p + 0x1EC, 2, 1, 0);
    SceneActors_SetHealthLimits(self, health, health, scale);
    PositionedEffect_InitAttackCollider(p);
    PositionedEffect_InitRadialAttackColliders(p);
    PositionedEffect_InitContactCollider(p);
    switch (mode) {
        case 0:
            setMode(self, 0, 0, 0);
            changeState(self, 1, ActorSoundPhase_StartFlaggedAnimation);
            p->attachment = Vanargand_CreatePartController((u8 *)p + 0x17C, p->palette, 1);
            break;
        case 1:
            setMode(self, 2, 0, 1);
            changeState(self, 26, Vanargand_OnPrimaryStopState);
            p->attachment = Vanargand_CreatePartController((u8 *)p + 0x17C, p->palette, 0);
            placeBody(self);
            FIELD(u16, p, 0x5C2) = 1;
            break;
        case 2:
            setMode(self, 2, 0, 0);
            changeState(self, 1, ActorSoundPhase_StartFlaggedAnimation);
            p->attachment = Vanargand_CreatePartController((u8 *)p + 0x17C, p->palette, 1);
            FIELD(u16, p, 0x5C2) = 0;
            break;
        case 3:
            setMode(self, 2, 0, 9);
            changeState(self, 29, Vanargand_OnAlternateStopState);
            p->attachment = Vanargand_CreatePartController((u8 *)p + 0x17C, p->palette, 0);
            CpuSet(gPaletteBankData + 0x1040, p->palette, 0x04000020);
            CpuSet(p->fadeB, gUnk_03004FC0, 0x04000068);
            Vanargand_ResetPaletteTransition(p);
            placeBody(self);
            FIELD(u16, p, 0x5C2) = 1;
            break;
    }
    if (!p->attachment)
        return -1;
    if (p->variant == 1) {
        PairedSceneActor_InitHalves(p->attachment, 0x60, 0x400, 300, FIELD(u16, p, 0x584), 0x80, 60,
                                    0, 3);
        Vanargand_SetPartAnimationParameter(p->attachment, 1);
    } else {
        PairedSceneActor_InitHalves(p->attachment, 0x60, 0x400, 300, FIELD(u16, p, 0x584), 0x80, 60,
                                    0, 1);
        Vanargand_SetPartAnimationParameter(p->attachment, 0);
    }
    PairedSceneActor_SetResetValues(p->attachment, limitA, limitB);
    Vanargand_SetPairedEffectParameter(p->attachment, effect);
    {
        void *callback = Vanargand_ApplyPendingDamage;
        FIELD(void *, self, 0x104) = callback;
    }
    {
        void *callback = Vanargand_ApplyRequestedMode;
        FIELD(void *, self, 0x108) = callback;
    }
    FIELD(void *, p, 0x57C) = VanargandProjectilePool_Create(FIELD(u16, p, 0x58C), 10);
    FIELD(u32, p, 0x574) = 0;
    FIELD(u32, p, 0x578) = 0;
    FIELD(u32, p, 0x534) = 0;
    FIELD(u8, p, 0x55D) = 0;
    FIELD(u8, p, 0x55C) = 0;
    FIELD(u32, p, 0x568) = 0;
    p->afterimages = VanargandAfterimagePool_Create();
    FIELD(u16, p, 0x5AA) = 0;
    FIELD(u32, p, 0x5A4) = 0;
    FIELD(u16, p, 0x5A8) = 0;
    FIELD(u16, p, 0x5AC) = 0;
    FIELD(u16, p, 0x5AE) = 0;
    FIELD(u16, p, 0x5B0) = 0;
    FIELD(u32, p, 0xF18) = 0;
    FIELD(u16, p, 0xF20) = 0;
    FIELD(u32, p, 0xF1C) = 0;
    FIELD(u16, p, 0xF22) = 0;
    FIELD(u8, p, 0x5C4) = 0;
    FIELD(u8, p, 0x5C5) = 0;
    FIELD(u8, p, 0x5C6) = 0;
    {
        s32 v = 3 - __divsi3(FIELD(u8, self, 0xE1), 43);
        asm("" : "+r"(v));
        FIELD(u8, p, 0x5C7) = v;
    }
    FIELD(u16, p, 0x5C2) = 0;
    FIELD(u16, p, 0x5BE) = 0;
    p->transition = 0;
    FIELD(u32, p, 0xF40) = 1;
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081AE0C0);
#endif
