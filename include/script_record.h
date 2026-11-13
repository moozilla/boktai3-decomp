#ifndef GUARD_SCRIPT_RECORD_H
#define GUARD_SCRIPT_RECORD_H
#include "script_options.h"
#include "modifier_list.h"
#include "scene_actor_style.h"
struct ScriptOptions *Modifier_GetTableTask(void);
s32 ScriptRecord_FindModifierRecord(struct ModifierRecord *, s32);
s32 Modifier_ScaleDefaultStat(struct StatModifier *, s32);
s32 Modifier_ScaleKindEightToTenStat(struct StatModifier *, s32);
s32 Modifier_ScaleKindFourStat(struct StatModifier *, s32);
s32 Modifier_ScaleKindSevenStat(struct StatModifier *, s32);
s32 Modifier_AddKindSixStat(struct StatModifier *, s32);
s32 Modifier_AddKindThreeStat(struct StatModifier *, s32);
s32 StatModifier_ScaleForGlobalMode(struct StatModifier *, s32);
s32 StatModifier_AddForGlobalMode(struct StatModifier *, s32);
s32 StatModifier_AddForOwnerMode(struct StatModifier *, s32);
s32 StatModifier_ScaleAtOwnerThreshold(struct StatModifier *, s32);
s32 CallbackList_RunMatchingState(struct ScenePoolActor *, s32, s32);
s32 CallbackList_RunStateEight(struct ScenePoolActor *);
u32 Modifier_UpdateIdleDrain(void *);
s32 ActorDrain_Update(struct ScenePoolActor *);
s32 Mod(s32, s32);
void TimedSpriteEffect_RequestStop(u32);
s8 SpriteSlots_AttachSource(void *);
u8 LinkParticipant_IsLocallySelected(void *);
static inline s32 ScaleModifierAmount(struct StatModifier *modifier, s32 value)
{
    s32 result;
    if (!modifier->state.enabled)
        return value;
    result = Div(modifier->state.record.amount * value, 10);
    if (result < 0)
        result = 1;
    return result;
}
static inline s32 AddModifierAmount(struct StatModifier *modifier, s32 value)
{
    s32 result;
    if (!modifier->state.enabled)
        return value;
    result = value + modifier->state.record.amount;
    if (result < 0)
        result = 1;
    return result;
}
#endif
