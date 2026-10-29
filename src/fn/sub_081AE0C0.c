#include "global.h"
#include "render.h"
#include "player.h"
#include "motion_track.h"

typedef void (*VanargandStateCallback)(void *);
typedef s32 (*VanargandUpdateCallback)(void *);

struct VanargandPart {
    u8 pad0[0x10];
    u16 state;
};
struct Vanargand {
    u8 unknown0[0x54];
    struct SceneVec position; /* 0x54 */
    u8 unknown5C[0x34];
    u32 actionTimer; /* 0x90 */
    s16 health; /* 0x94 */
    u8 unknown96[0x6];
    u32 actorFlags; /* 0x9C */
    u8 unknownA0[0x2];
    u8 phase; /* 0xA2 */
    u8 unknownA3[0x1];
    u32 timer; /* 0xA4 */
    u8 unknownA8[0x3];
    u8 state; /* 0xAB */
    u8 mode; /* 0xAC */
    u8 step; /* 0xAD */
    u8 actionPhase; /* 0xAE */
    u8 finished; /* 0xAF */
    u8 entered; /* 0xB0 */
    u8 pendingAnimation; /* 0xB1 */
    u8 ready; /* 0xB2 */
    u8 unknownB3[0x11];
    u32 modeTimer; /* 0xC4 */
    u8 unknownC8[0x19];
    u8 healthRatio; /* 0xE1 */
    u8 unknownE2[0x1E];
    VanargandStateCallback callback; /* 0x100 */
    VanargandUpdateCallback damageCallback; /* 0x104 */
    VanargandUpdateCallback modeCallback; /* 0x108 */
    struct Player *targetPlayer; /* 0x10C */
    struct SceneVec *targetPosition; /* 0x110 */
    u8 unknown114[0x60];
    u8 backdrop[8]; /* 0x174 */
    u8 partController[8]; /* 0x17C */
    u8 spriteAnchor[8]; /* 0x184 */
    u8 animation[0x60]; /* 0x18C */
    u8 drawable[0x28]; /* 0x1EC */
    u8 spriteParameter[8]; /* 0x214 */
    u8 actionSequence[0xb4]; /* 0x21C */
    u8 spriteGroup[0x12c]; /* 0x2D0 */
    struct VanargandPart part; /* 0x3FC */
    u8 unknown410[0x8];
    struct SceneVec partPosition; /* 0x418 */
    u8 unknown420[0xE0];
    u16 partActive; /* 0x500 */
    u8 unknown502[0x1C];
    u16 attackParameter; /* 0x51E */
    u8 unknown520[0x14];
    struct MotionTrack track; /* 0x534 */
    void *attachment; /* 0x56C */
    s32 targetCount; /* 0x570 */
    u32 counter574; /* 0x574 */
    u32 impactShake; /* 0x578 */
    void *projectilePool; /* 0x57C */
    u8 unknown580[0x4];
    u16 pairedMotion; /* 0x584 */
    u16 parameter586; /* 0x586 */
    u16 parameter588; /* 0x588 */
    u16 parameter58A; /* 0x58A */
    u16 projectileCount; /* 0x58C */
    u16 parameter58E; /* 0x58E */
    u8 unknown590[0x8];
    u16 scriptS[3]; /* 0x598 */
    u16 thresholds[3]; /* 0x59E */
    u32 hitFlags; /* 0x5A4 */
    s16 damage; /* 0x5A8 */
    u16 field5AA; /* 0x5AA */
    s16 accumulatedDamage; /* 0x5AC */
    u16 flashTimer; /* 0x5AE */
    u16 delay; /* 0x5B0 */
    u8 unknown5B2[0x6];
    u16 field5B8; /* 0x5B8 */
    u16 field5BA; /* 0x5BA */
    u8 unknown5BC[0x2];
    u16 field5BE; /* 0x5BE */
    u8 unknown5C0[0x2];
    s16 presentationMode; /* 0x5C2 */
    u8 phase5C4; /* 0x5C4 */
    u8 phase5C5; /* 0x5C5 */
    u8 phase5C6; /* 0x5C6 */
    u8 healthBand; /* 0x5C7 */
    u8 unknown5C8[0x150];
    u8 palette[0x200]; /* 0x718 */
    u8 fadeA[0x200]; /* 0x918 */
    u8 fadeB[0x200]; /* 0xB18 */
    u8 fadeC[0x200]; /* 0xD18 */
    s32 paletteFrame; /* 0xF18 */
    u32 paletteSelector; /* 0xF1C */
    u16 paletteMode; /* 0xF20 */
    u16 paletteShift; /* 0xF22 */
    u32 variant; /* 0xF24 */
    u32 scriptE[3]; /* 0xF28 */
    s32 transition; /* 0xF34 */
    u32 eventParameter; /* 0xF38 */
    u8 unknownF3C[0x4];
    u32 enabledF40; /* 0xF40 */
    void *afterimages; /* 0xF44 */
};
s32 __divsi3(s32, s32);
void Vanargand_ResetPaletteTransition(struct Vanargand *);
void CpuSet(const void *, void *, u32);

static inline s32 InterpolatePaletteChannel(s32 start, s32 end, s32 frame, s32 duration)
{
    s32 value;
    if (frame <= 0)
        return start;
    if (frame >= duration)
        return end;
    value = ((end - start) << 8) * frame / duration;
    if (value >= 0)
        value >>= 8;
    else
        value = -(-value >> 8);
    return start + value;
}

void Vanargand_StopActionSequence(void *);
static inline void SetStagePaletteColors(s32 stage)
{
    s32 red, green, blue;
    u16 color;
    red = InterpolatePaletteChannel(7, 19, stage, 3);
    green = InterpolatePaletteChannel(0, 18, stage, 3);
    blue = InterpolatePaletteChannel(0, 19, stage, 3);
    color = red | (green << 5) | (blue << 10);
    gBgPaletteBuffer[0x56 / 2] = color;
    gBgPaletteBuffer[0x10c / 2] = color;
    {
        s32 red, green, blue;
        u16 nextColor;
        red = InterpolatePaletteChannel(21, 24, stage, 3);
        green = InterpolatePaletteChannel(0, 29, stage, 3);
        blue = InterpolatePaletteChannel(0, 31, stage, 3);
        nextColor = red | (green << 5) | (blue << 10);
        gBgPaletteBuffer[0x5a / 2] = nextColor;
        gBgPaletteBuffer[0x10e / 2] = nextColor;
    }
}

/* A stored preset is resolved by reference; dynamic restoration already has a bank value. */
struct VanargandPaletteSelection {
    s32 bank;
};
static inline void ApplyPaletteBank(struct Vanargand *p, struct Vanargand *self, s32 bank)
{
    switch (bank) {
        case 0x6F: {
            CpuSet(gPaletteBankData + 0xDE0, p->palette, 0x04000020);
            CpuSet(p->fadeA, gBgPaletteBuffer, 0x04000068);
            SetStagePaletteColors(p->transition);
            break;
        }
        case 0x82:
            CpuSet(gPaletteBankData + 0x1040, p->palette, 0x04000020);
            CpuSet(p->fadeB, gBgPaletteBuffer, 0x04000068);
            break;
        case 0x7F:
            CpuSet(gPaletteBankData + 0xFE0, p->palette, 0x04000018);
            CpuSet(p->fadeC, gBgPaletteBuffer, 0x04000068);
            break;
        default:
            CpuSet(gPaletteBankData + 0x3240, self->palette, 0x04000008);
            break;
    }
    Vanargand_ResetPaletteTransition(self);
}
static inline void ApplyPaletteSelection(struct Vanargand *p, struct Vanargand *self, const struct VanargandPaletteSelection *selection)
{
    ApplyPaletteBank(p, self, selection->bank);
}
static inline void RestorePalette(struct Vanargand *p, struct Vanargand *self)
{
    s32 bank = 0x6F;
    if (!p->variant)
        bank = 0x7F;
    ApplyPaletteBank(p, self, bank);
}
static inline void ChangeState(struct Vanargand *p, s32 state, VanargandStateCallback callback)
{
    struct Vanargand *self = p;
    if (p->state == 3) {
        RestorePalette(p, self);
        Vanargand_StopActionSequence((void *)&self->actionSequence);
    }
    {
        u32 zero = 0;
        p->state = state;
        p->callback = callback;
        p->actionTimer = zero;
        p->pendingAnimation = 1;
        p->phase = zero;
        p->timer = zero;
    }
}

s32 Script_GetKeywordValue(u32, s32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
static inline u16 ReadScriptHalfword(void)
{
    return Script_GetValue();
}
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
void ActorSoundPhase_StartFlaggedAnimation(void *);
void Vanargand_OnPrimaryStopState(void *);
void Vanargand_OnAlternateStopState(void *);
void *Vanargand_CreatePartController(void *, void *, s32);
void PairedSceneActor_InitHalves(void *, s32, s32, s32, s32, s32, s32, s32, s32);
void Vanargand_SetPartAnimationParameter(void *, s32);
void PairedSceneActor_SetResetValues(void *, s32, s32);
void Vanargand_SetPairedEffectParameter(void *, s32);
s32 Vanargand_ApplyPendingDamage(void *);
s32 Vanargand_ApplyRequestedMode(void *);
void *VanargandProjectilePool_Create(s32, s32);
void *VanargandAfterimagePool_Create(void);
struct InitPosition {
    s32 x : 16, y : 16, z : 16;
};
static inline void SetMode(struct Vanargand *p, s32 mode, s32 step, s32 phase)
{
    p->mode = mode;
    p->step = step;
    p->actionPhase = phase;
    p->modeTimer = 0;
    p->entered = 1;
}
static inline void SetPosition(struct SceneVec *position, s32 x, s32 y, s32 z)
{
    position->x = x;
    position->y = y;
    position->z = z;
}

// Initializes script parameters, palettes, paired parts and the selected starting mode.
s32 Vanargand_Init(struct Vanargand *p, void *arg)
{
    struct Vanargand *self = p;
    struct InitPosition position;
    s32 healthParameter;
    s32 type = Script_GetKeywordValue('t', 5);
    s32 health;
    s32 limitA, limitB;
    s32 effect;
    s32 mode;
    s32 facing;
    s32 i;
    if (type != 5)
        return -1;
    SceneActors_Register(p, 5);
    p->targetCount = 1;

    {
        struct SceneVec **positions = &p->targetPosition;
        struct Player **targets = &p->targetPlayer;
        struct Player **players = gPlayers;
        s32 count = p->targetCount;
        for (i = 0; i < count; i++) {
            struct Player *value = players[i];
            if (value) {
                targets[i] = value;
                positions[i] = &value->position;
            } else {
                targets[i] = 0;
                positions[i] = 0;
            }
        }
    }
    if (Script_SeekToKeyword('s')) {
        Script_GetValue();
        healthParameter = Script_GetValue();
    } else
        healthParameter = 100;
    if (Script_SeekToKeyword('a')) {
        p->pairedMotion = ReadScriptHalfword();
        p->parameter586 = ReadScriptHalfword();
        p->parameter588 = ReadScriptHalfword();
        p->parameter58A = ReadScriptHalfword();
        p->projectileCount = ReadScriptHalfword();
        p->parameter58E = ReadScriptHalfword();
    } else {
        p->pairedMotion = 1;
        p->parameter586 = 2;
        p->parameter588 = 4;
        p->parameter58A = 8;
        p->projectileCount = 16;
        p->parameter58E = 32;
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
        u16 *out = p->scriptS;
        for (i = 2; i >= 0; i--)
            *out++ = ReadScriptHalfword();
    } else {
        s32 value = 200;
        for (i = 2; i >= 0; i--)
            p->scriptS[i] = value;
    }
    if (Script_SeekToKeyword('V')) {
        u16 *out = p->thresholds;
        for (i = 2; i >= 0; i--)
            *out++ = ReadScriptHalfword();
    } else {
        s32 value = 200;
        for (i = 2; i >= 0; i--)
            p->thresholds[i] = value;
    }
    if (Script_SeekToKeyword('W')) {
        Script_GetValue();
        p->attackParameter = ReadScriptHalfword();
        effect = Script_GetValue();
    } else {
        p->attackParameter = 0;
        effect = 0;
    }
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
        facing = 32;
    } else {
        position.x = 640;
        position.y = 768;
        position.z = 640;
        facing = 32;
    }
    if (Script_SeekToKeyword('m'))
        p->variant = Script_GetValue();
    else
        p->variant = 1;
    p->eventParameter = Script_GetKeywordValue('E', 0);
    mode = Script_GetKeywordValue('D', 0);
    if (Script_SeekToKeyword('e')) {
        u32 *out = p->scriptE;
        for (i = 2; i >= 0; i--)
            *out++ = Script_GetValue();
    } else {
        s32 value = 0;
        for (i = 2; i >= 0; i--)
            p->scriptE[i] = value;
    }
    if (Script_SeekToKeyword('c')) {
        s32 a = Script_GetValue();
        s32 b = Script_GetValue();
        SceneActors_SetEventScripts(self, a, b);
    } else
        SceneActors_SetEventScripts(self, 0, 0);
    SceneActors_Initialize(self, arg, type, &position, facing);
    CpuSet(gPaletteBankData + 0xDE0, p->palette, 0x04000080);
    CpuSet(ScriptProc_Start(0x92B3, 0x454B) + 0x14, p->fadeA, 0x04000080);
    CpuSet(ScriptProc_Start(0x92B3, 0xEEE9) + 0x14, p->fadeB, 0x04000080);
    CpuSet(ScriptProc_Start(0x92B3, 0xF392) + 0x14, p->fadeC, 0x04000080);
    SceneBackdrop_Initialize(p, (void *)&p->backdrop);
    Vanargand_InitAnimatedAttachment(p, (void *)&p->animation);
    Vanargand_InitSpriteGroup(p, (void *)&p->spriteGroup, (void *)&p->spriteAnchor, (void *)&p->spriteParameter);
    if (p->variant)
        Vanargand_InitPart(p, (void *)&p->part);
    else {
        SetPosition(&p->partPosition, 0x33B, 0x100, 0x33B);
    }
    RestorePalette(p, p);
    SceneActors_StartUnmirroredAnimation(self, (void *)&p->animation, (void *)&p->drawable, 2, 1, 0);
    SceneActors_SetHealthLimits(self, health, health, healthParameter);
    PositionedEffect_InitAttackCollider(p);
    PositionedEffect_InitRadialAttackColliders(p);
    PositionedEffect_InitContactCollider(p);
    switch (mode) {
        case 0:
            SetMode(self, 0, 0, 0);
            ChangeState(self, 1, ActorSoundPhase_StartFlaggedAnimation);
            p->attachment = Vanargand_CreatePartController((void *)&p->partController, p->palette, 1);
            break;
        case 1:
            SetMode(self, 2, 0, 1);
            ChangeState(self, 26, Vanargand_OnPrimaryStopState);
            p->attachment = Vanargand_CreatePartController((void *)&p->partController, p->palette, 0);
            SetPosition(&self->position, 0x33B, 0x100, 0x33B);
            p->presentationMode = 1;
            break;
        case 2:
            SetMode(self, 2, 0, 0);
            ChangeState(self, 1, ActorSoundPhase_StartFlaggedAnimation);
            p->attachment = Vanargand_CreatePartController((void *)&p->partController, p->palette, 1);
            p->presentationMode = 0;
            break;
        case 3:
            SetMode(self, 2, 0, 9);
            ChangeState(self, 29, Vanargand_OnAlternateStopState);
            p->attachment = Vanargand_CreatePartController((void *)&p->partController, p->palette, 0);
            {
                struct VanargandPaletteSelection selection;
                selection.bank = 0x82;
                ApplyPaletteSelection(p, p, &selection);
            }
            SetPosition(&self->position, 0x33B, 0x100, 0x33B);
            p->presentationMode = 1;
            break;
    }
    if (!p->attachment)
        return -1;
    if (p->variant == 1) {
        PairedSceneActor_InitHalves(p->attachment, 0x60, 0x400, 300, p->pairedMotion, 0x80, 60,
                                    0, 3);
        Vanargand_SetPartAnimationParameter(p->attachment, 1);
    } else {
        PairedSceneActor_InitHalves(p->attachment, 0x60, 0x400, 300, p->pairedMotion, 0x80, 60,
                                    0, 1);
        Vanargand_SetPartAnimationParameter(p->attachment, 0);
    }
    PairedSceneActor_SetResetValues(p->attachment, limitA, limitB);
    Vanargand_SetPairedEffectParameter(p->attachment, effect);
    {
        VanargandUpdateCallback callback = Vanargand_ApplyPendingDamage;
        self->damageCallback = callback;
    }
    {
        VanargandUpdateCallback callback = Vanargand_ApplyRequestedMode;
        self->modeCallback = callback;
    }
    p->projectilePool = VanargandProjectilePool_Create(p->projectileCount, 10);
    p->counter574 = 0;
    p->impactShake = 0;
    MotionTrack_Reset(&p->track);
    p->afterimages = VanargandAfterimagePool_Create();
    p->field5AA = 0;
    p->hitFlags = 0;
    p->damage = 0;
    p->accumulatedDamage = 0;
    p->flashTimer = 0;
    p->delay = 0;
    p->paletteFrame = 0;
    p->paletteMode = 0;
    p->paletteSelector = 0;
    p->paletteShift = 0;
    p->phase5C4 = 0;
    p->phase5C5 = 0;
    p->phase5C6 = 0;
    {
        s32 v = 3 - __divsi3(self->healthRatio, 43);
        p->healthBand = v;
    }
    p->presentationMode = 0;
    p->field5BE = 0;
    p->transition = 0;
    p->enabledF40 = 1;
    return 0;
}
