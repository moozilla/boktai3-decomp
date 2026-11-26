#include "global.h"
#include "scene/areas/area_effect_link.h"
#include "scene/actor/scene_actor_style.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "minigames/coffin_bike/coffin_bike.h"

struct LinkStylePayload {
    struct SceneActorStyle style;
    u8 unk_1C[8];
};
void SceneActorStyle_InitializeFromGameState(void *);
void Modifier_InitSourceInfo(void *, void *);
void ModifierAction_InitializeSecondary(void *, void *);
void StatModifier_InitializeCallbacks(void *, void *);
void Actor_RecomputeScriptedValue(void *);
void ActorStats_ConfigureScaledParameters(void *, s32, s32, s32, s32, s32, s32);
void ActorAction_InitializeMode(void *, void *, s32);
s32 ModifierStats_ApplyKind3(void *, s32);
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

static inline struct AreaEffectLinkState *CoffinBike_ReadStyleSession(struct CoffinBike *glove,
                                                                s32 *participant)
{
    s32 index = glove->participant;
    struct AreaEffectLinkState *session;
    *participant = index;
    session = gLinkSession;
    if (!session || session->unavailable != 1)
        return NULL;
    return session;
}
void CoffinBike_InitializeStyleModifiers(struct CoffinBike *p)
{
    struct CoffinBikeMotion *self = &p->motion;
    u32 mask = 0x800;
    if ((gEngineFlags | gScenePauseFlags) & mask) {
        s32 player;
        struct AreaEffectLinkState *link;
        struct LinkStylePayload *payload;
        s32 i;
        link = CoffinBike_ReadStyleSession(p, &player);
        if (!link || !ValidParticipant(player))
            return;
        payload = (struct LinkStylePayload *)(link->incomingPayload +
                                              player * sizeof(struct LinkStylePayload));
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
    self->primaryValue = ModifierStats_ApplyKind3(self, self->primaryValue);
    if (self->primaryKind == 5)
        ActorEffect_EmitOffsetPair(self, &p->model, &self->primaryCallback,
                                   Character_OnActionSoundCue, p->participant + 1);
    Modifier_InitDrainState(self->drain, p->model.parts[0].drainSource, 120, 1);
    Modifier_InitFourthSourceEffect(self, p);
    self->fourthValue = ModifierStats_ApplyKind3(self, self->fourthValue);
    Character_SetLowResourceColorEffect(&self->emitter, &self->style);
}
