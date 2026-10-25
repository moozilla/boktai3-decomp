// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "area_effect_link.h"
#include "scene_actor_style.h"

struct GloveMotion {
    u8 unk_00[0x44];
    u8 colorState[4];
    struct SceneActorStyle style;
    u8 unk_64[0xB0 - 0x64];
    u8 drain[0x10];
    u8 unk_C0[0x2B0 - 0xC0];
    u8 actionSource[5];
    u8 actionMode;
    u8 unk_2B6[4];
    u16 actionValue;
    u8 unk_2BC[0x31E - 0x2BC];
    u16 fourthValue;
    u8 unk_320[0x5F8 - 0x320];
};
struct Glove {
    u8 unk_00[0x18];
    struct GloveMotion motion;
    u8 effectSource[0x20];
    u8 drainSource[0x862 - 0x630];
    u8 stylePlayer;
    u8 unk_863[0x8F8 - 0x863];
    s8 participant;
};
struct LinkStylePayload {
    struct SceneActorStyle style;
    u8 unk_1C[8];
};
extern u32 gUnk_030053F4, gUnk_030053F0;
void SceneActorStyle_InitializeFromGameState(void *);
void Modifier_InitSourceInfo(void *, void *);
void ModifierAction_InitializeSecondary(void *, void *);
void StatModifier_InitializeCallbacks(void *, void *);
void Actor_RecomputeScriptedValue(void *);
void ActorStats_ConfigureScaledParameters(void *, s32, s32, s32, s32, s32, s32);
void ActorAction_InitializeMode(void *, void *, s32);
s32 sub_0811EAB8(void *, s32);
void Character_OnActionSoundCue(void);
void ActorEffect_EmitOffsetPair(void *, void *, void *, void (*)(void), s32);
void Modifier_InitDrainState(void *, void *, u16, u8);
void Modifier_InitFourthSourceEffect(void *, void *);
void Character_SetLowResourceColorEffect(void *, void *);
static inline u8 ValidParticipant(s32 i)
{
    if (i < 0)
        goto invalid;
    if (i <= 1)
        goto valid;
invalid:
    return 0;
valid:
    return 1;
}

// Tentative: initializes the Glove actor's style, stats, and modifier effects.

void Glove_InitializeStyleModifiers(struct Glove *p)
{
    struct GloveMotion *self = &p->motion;
    u32 mask = 0x800;
    if ((gUnk_030053F4 | gUnk_030053F0) & mask) {
        s32 player = p->participant;
        register struct AreaEffectLinkState *link asm("r1") = gUnk_020004B4;
        struct LinkStylePayload *payload;
        s32 i;
        if (!link || link->unavailable != 1 || !ValidParticipant(player))
            return;
        payload = (struct LinkStylePayload *)(link->incomingPayload + player * 0x24);
        if (!payload)
            return;
        self->style.adjustedA = payload->style.adjustedA;
        self->style.adjustedC = payload->style.adjustedC;
        self->style.adjustedB = payload->style.adjustedB;
        self->style.parameterE = payload->style.parameterE;
        self->style.initialHalfword = payload->style.initialHalfword;
        self->style.currentHalfword = payload->style.currentHalfword;
        self->style.parameterC = payload->style.parameterC;
        self->style.parameterA = payload->style.parameterA;
        self->style.parameterF = payload->style.parameterF;
        self->style.count = payload->style.count;
        for (i = 0; i < self->style.count; i++)
            self->style.entries[i] = payload->style.entries[i];
        self->style.parameterB = payload->style.parameterB;
        self->style.parameterD = payload->style.parameterD;
        p->stylePlayer = p->participant;
    } else {
        SceneActorStyle_InitializeFromGameState(&self->style);
        p->stylePlayer = p->participant;
    }
    Modifier_InitSourceInfo(self, p);
    ModifierAction_InitializeSecondary(self, p);
    StatModifier_InitializeCallbacks(self, p);
    Actor_RecomputeScriptedValue(self);
    ActorStats_ConfigureScaledParameters(self, 0x1000, 0x3000, -0x2000, 0x2000, 0, 0x1000);
    ActorAction_InitializeMode(self, p, 0);
    self->actionValue = sub_0811EAB8(self, self->actionValue);
    if (self->actionMode == 5)
        ActorEffect_EmitOffsetPair(self, p->effectSource, self->actionSource,
                                   Character_OnActionSoundCue, p->participant + 1);
    Modifier_InitDrainState(self->drain, p->drainSource, 120, 1);
    Modifier_InitFourthSourceEffect(self, p);
    self->fourthValue = sub_0811EAB8(self, self->fourthValue);
    Character_SetLowResourceColorEffect(self->colorState, &self->style);
}
