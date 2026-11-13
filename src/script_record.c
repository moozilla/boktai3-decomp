#include "script_record.h"

s32 ScriptRecord_FindModifierRecord(struct ModifierRecord *out, s32 id)
{
    struct ScriptOptions *cursor;
    struct ScriptOptions *table = (void *)gScriptRecordTable;
    s32 entryIndex;
    if (!table)
        return -1;
    cursor = table;
    for (entryIndex = 0; entryIndex < cursor->entryCount; entryIndex++) {
        if ((s8)cursor->entries[entryIndex].kind == id) {
            out->order = (s8)cursor->entries[entryIndex].kind;
            out->kind = cursor->entries[entryIndex].value;
            out->amount = cursor->entries[entryIndex].parameter;
            out->extra = cursor->entries[entryIndex].mode;
            return entryIndex;
        }
    }
    return -1;
}

s32 ScriptRecord_GetEntryOption(void)
{
    if (!((struct ScriptOptions *)gScriptRecordTable))
        return 0;
    return ((struct ScriptOptions *)gScriptRecordTable)->option;
}

s32 ScriptRecord_FindModifier(struct ScriptTargetEntry *out, s32 id)
{
    struct ScriptOptions *cursor;
    struct ScriptOptions *table = (void *)gScriptRecordTable;
    s32 entryIndex;
    if (!table)
        return -1;
    cursor = table;
    for (entryIndex = 0; entryIndex < cursor->targetCount; entryIndex++) {
        if ((s8)cursor->targets[entryIndex].kind == id) {
            out->kind = cursor->targets[entryIndex].kind;
            out->parameters[0] = cursor->targets[entryIndex].parameters[0];
            out->parameters[1] = cursor->targets[entryIndex].parameters[1];
            out->parameters[2] = cursor->targets[entryIndex].parameters[2];
            out->value = cursor->targets[entryIndex].value;
            return entryIndex;
        }
    }
    return -1;
}

s32 ScriptRecord_GetTargetOption(void)
{
    s32 count;
    if (gScriptRecordTable)
        count = ((struct ScriptOptions *)gScriptRecordTable)->targetOption;
    else
        count = 0;
    return count;
}

s32 ScriptRecord_FindExtendedModifier(struct ScriptWideEntry *out, s32 id)
{
    struct ScriptOptions *cursor;
    struct ScriptOptions *table = (void *)gScriptRecordTable;
    s32 entryIndex;
    if (!table)
        return -1;
    cursor = table;
    for (entryIndex = 0; entryIndex < cursor->wideCount; entryIndex++) {
        if ((s8)cursor->wideEntries[entryIndex].kind == id) {
            out->kind = cursor->wideEntries[entryIndex].kind;
            out->value = cursor->wideEntries[entryIndex].value;
            out->parameters[0] = cursor->wideEntries[entryIndex].parameters[0];
            out->parameters[1] = cursor->wideEntries[entryIndex].parameters[1];
            out->parameters[2] = cursor->wideEntries[entryIndex].parameters[2];
            out->parameters[3] = cursor->wideEntries[entryIndex].parameters[3];
            out->parameterC = cursor->wideEntries[entryIndex].parameterC;
            out->counter = 0;
            return entryIndex;
        }
    }
    return -1;
}

s32 ScriptRecord_GetWideOption(void)
{
    s32 count;
    if (gScriptRecordTable)
        count = ((struct ScriptOptions *)gScriptRecordTable)->wideOption;
    else
        count = 0;
    return count;
}

s32 ScriptRecord_FindAttributeModifier(struct ScriptShortEntry *out, s32 id)
{
    struct ScriptOptions *cursor;
    struct ScriptOptions *table = (void *)gScriptRecordTable;
    s32 entryIndex;
    if (!table)
        return -1;
    cursor = table;
    for (entryIndex = 0; entryIndex < cursor->shortCount; entryIndex++) {
        if ((s8)cursor->shortEntries[entryIndex].kind == id) {
            out->kind = cursor->shortEntries[entryIndex].kind;
            out->value = cursor->shortEntries[entryIndex].value;
            out->parameters[0] = cursor->shortEntries[entryIndex].parameters[0];
            out->parameters[1] = cursor->shortEntries[entryIndex].parameters[1];
            out->parameters[2] = cursor->shortEntries[entryIndex].parameters[2];
            out->parameters[3] = cursor->shortEntries[entryIndex].parameters[3];
            return entryIndex;
        }
    }
    return -1;
}

s32 ScriptRecord_GetShortOption(void)
{
    s32 count;
    if (gScriptRecordTable)
        count = ((struct ScriptOptions *)gScriptRecordTable)->shortOption;
    else
        count = 0;
    return count;
}

s32 ScriptRecord_FindByteModifier(struct ScriptByteEntry *out, s32 id)
{
    struct ScriptOptions *cursor;
    struct ScriptOptions *table = (void *)gScriptRecordTable;
    s32 entryIndex;
    if (!table)
        return -1;
    cursor = table;
    for (entryIndex = 0; entryIndex < cursor->byteCount; entryIndex++) {
        if ((s8)cursor->byteEntries[entryIndex].kind == id) {
            out->kind = cursor->byteEntries[entryIndex].kind;
            out->category = cursor->byteEntries[entryIndex].category;
            out->parameterA = cursor->byteEntries[entryIndex].parameterA;
            out->parameterB = cursor->byteEntries[entryIndex].parameterB;
            out->value = cursor->byteEntries[entryIndex].value;
            return entryIndex;
        }
    }
    return -1;
}

s32 ScriptRecord_GetByteOption(void)
{
    struct ScriptOptions *task = Modifier_GetTableTask();
    if (!task)
        return 0;
    return task->byteOption;
}

s32 Modifier_ScaleDefaultStat(struct StatModifier *modifier, s32 value)
{
    return ScaleModifierAmount(modifier, value);
}

s32 Modifier_ScaleKindEightToTenStat(struct StatModifier *modifier, s32 value)
{
    return ScaleModifierAmount(modifier, value);
}

s32 Modifier_ScaleKindFourStat(struct StatModifier *modifier, s32 value)
{
    return ScaleModifierAmount(modifier, value);
}

s32 Modifier_ScaleKindSevenStat(struct StatModifier *modifier, s32 value)
{
    return ScaleModifierAmount(modifier, value);
}

s32 Modifier_AddKindSixStat(struct StatModifier *modifier, s32 value)
{
    return AddModifierAmount(modifier, value);
}

s32 Modifier_AddKindThreeStat(struct StatModifier *modifier, s32 value)
{
    return AddModifierAmount(modifier, value);
}

s32 StatModifier_ScaleForGlobalMode(struct StatModifier *modifier_, s32 value)
{
    struct StatModifier *modifier = modifier_;
    s32 result;
    s32 input = value;
    if (modifier->state.enabled == 0)
        do {
            return input;
        } while (0);
    {
        u16 mask = 0x800;
        if (!((gEngineFlags | gScenePauseFlags) & mask) || modifier->state.owner->mode == 1)
            do {
                return input;
            } while (0);
    }
    result = Div(modifier->state.record.amount * input, 10);
    if (result < 0)
        result = 1;
    return result;
}

s32 StatModifier_AddForGlobalMode(struct StatModifier *arg, s32 value)
{
    struct StatModifier *modifier = arg;
    s32 result;
    s32 input = value;
    if (modifier->state.enabled == 0)
        do {
            return input;
        } while (0);
    {
        u16 mask = 0x800;
        if (!((gEngineFlags | gScenePauseFlags) & mask) || modifier->state.owner->mode == 1)
            do {
                return input;
            } while (0);
    }
    result = input + modifier->state.record.amount;
    if (result < 0)
        result = 1;
    return result;
}

s32 StatModifier_AddForOwnerMode(struct StatModifier *modifier_, s32 value_)
{
    struct StatModifier *modifier = modifier_;
    s32 value = value_;
    s32 result;
    if (modifier->state.enabled == 0)
        do {
            result = value;
        } while (0);
    else {
        u32 mask = 0x800;
        if (!((gEngineFlags | gScenePauseFlags) & mask) || modifier->state.owner->mode == 0)
            do {
                result = value;
            } while (0);
        else {
            result = value + modifier->state.record.amount;
            if (result < 0)
                result = 1;
        }
    }
    return result;
}

s32 StatModifier_ScaleAtOwnerThreshold(struct StatModifier *modifier, s32 value)
{
    s32 result;
    struct StatModifierOwnerStats *stats;
    if (!modifier->state.enabled)
        return value;
    stats = &modifier->state.owner->stats;
    if (stats->current < stats->threshold)
        return value;
    result = Div(modifier->state.record.amount * value, 10);
    if (result < 0)
        result = 1;
    return result;
}

static inline int ClampAmount(int x)
{
    if (x > 999)
        x = 999;
    else if (x < 0)
        x = 0;
    return x;
}

void Actor_RecomputeScriptedValue(struct ScenePoolActor *actor)
{
    s32 i;
    struct StatModifier *modifiers = actor->modifiers;
    struct StatModifier *entry;
    struct StatModifier *cursor;
    actor->amountLimit = (*(&actor))->baseAmount;
    i = 0;
    if (i < actor->modifierCount) {
        entry = modifiers;
        cursor = entry;
        do {
            if (entry->state.record.kind == 6)
                actor->amountLimit = entry->callback(cursor, actor->amountLimit);
            entry++;
            cursor++;
            i++;
        } while (i < actor->modifierCount);
    }
    actor->amountLimit = ClampAmount(actor->amountLimit);
    actor->currentAmount = actor->amountLimit;
    actor->halfAmount = actor->amountLimit >> 1;
}

s32 ModifierStats_ApplyKind0(struct ScenePoolActor *stats, s32 value)
{
    struct StatModifier *base = (*&stats)->modifiers;
    s32 i = 0;
    if (i < stats->modifierCount) {
        struct StatModifier *modifier = base;
        struct StatModifier *entry = base;
        do {
            if (entry->state.record.kind == 0)
                value = entry->callback(modifier, value);
            entry++;
            modifier++;
            i++;
        } while (i < stats->modifierCount);
    }
    return value;
}

s32 ModifierStats_ApplyKind12(struct ScenePoolActor *stats, s32 value)
{
    struct StatModifier *base = (*&stats)->modifiers;
    s32 i = 0;
    if (i < stats->modifierCount) {
        struct StatModifier *modifier = base;
        struct StatModifier *entry = base;
        do {
            if (entry->state.record.kind == 12)
                value = entry->callback(modifier, value);
            entry++;
            modifier++;
            i++;
        } while (i < stats->modifierCount);
    }
    return value;
}

s32 ModifierStats_ApplyKind15(struct ScenePoolActor *stats, s32 value)
{
    struct StatModifier *base = (*&stats)->modifiers;
    s32 i = 0;
    if (i < stats->modifierCount) {
        struct StatModifier *modifier = base;
        struct StatModifier *entry = base;
        do {
            if (entry->state.record.kind == 15)
                value = entry->callback(modifier, value);
            entry++;
            modifier++;
            i++;
        } while (i < stats->modifierCount);
    }
    return value;
}

s32 ModifierStats_ApplyKind7(struct ScenePoolActor *stats, s32 value)
{
    struct StatModifier *base = (*&stats)->modifiers;
    s32 i = 0;
    if (i < stats->modifierCount) {
        struct StatModifier *modifier = base;
        struct StatModifier *entry = base;
        do {
            if (entry->state.record.kind == 7)
                value = entry->callback(modifier, value);
            entry++;
            modifier++;
            i++;
        } while (i < stats->modifierCount);
    }
    return value;
}

s32 ModifierStats_ApplyKind11(struct ScenePoolActor *stats, s32 value)
{
    struct StatModifier *base = (*&stats)->modifiers;
    s32 i = 0;
    if (i < stats->modifierCount) {
        struct StatModifier *modifier = base;
        struct StatModifier *entry = base;
        do {
            if (entry->state.record.kind == 11)
                value = entry->callback(modifier, value);
            entry++;
            modifier++;
            i++;
        } while (i < stats->modifierCount);
    }
    return value;
}

s32 ModifierStats_ApplyKinds4And14(struct ScenePoolActor *stats, s32 value)
{
    struct StatModifier *base = (*&stats)->modifiers;
    s32 i = 0;
    if (i < stats->modifierCount) {
        struct StatModifier *modifier = base;
        struct StatModifier *entry = base;
        do {
            if (entry->state.record.kind == 4 || entry->state.record.kind == 14)
                value = entry->callback(modifier, value);
            entry++;
            modifier++;
            i++;
        } while (i < stats->modifierCount);
    }
    return value;
}

s32 ModifierStats_ApplyKinds2And5(struct ScenePoolActor *stats, s32 value)
{
    struct StatModifier *base = (*&stats)->modifiers;
    s32 i = 0;
    if (i < stats->modifierCount) {
        struct StatModifier *modifier = base;
        struct StatModifier *entry = base;
        do {
            if (entry->state.record.kind == 2 || entry->state.record.kind == 5)
                value = entry->callback(modifier, value);
            entry++;
            modifier++;
            i++;
        } while (i < stats->modifierCount);
    }
    return value;
}

s32 ModifierStats_ApplyKind1(struct ScenePoolActor *stats, s32 value)
{
    struct StatModifier *base = (*&stats)->modifiers;
    s32 i = 0;
    if (i < stats->modifierCount) {
        struct StatModifier *modifier = base;
        struct StatModifier *entry = base;
        do {
            if (entry->state.record.kind == 1)
                value = entry->callback(modifier, value);
            entry++;
            modifier++;
            i++;
        } while (i < stats->modifierCount);
    }
    return value;
}

/* ModifierStats_ApplyKind3: address symbol retained for the external motion caller. */

s32 sub_0811EAB8(struct ScenePoolActor *stats, s32 value)
{
    struct StatModifier *base = (*&stats)->modifiers;
    s32 i = 0;
    if (i < stats->modifierCount) {
        struct StatModifier *modifier = base;
        struct StatModifier *entry = base;
        do {
            if (entry->state.record.kind == 3)
                value = entry->callback(modifier, value);
            entry++;
            modifier++;
            i++;
        } while (i < stats->modifierCount);
    }
    return value;
}

s32 CallbackList_RunMatchingState(struct ScenePoolActor *list, s32 value, s32 state)
{
    struct StatModifier *base = (*&list)->modifiers;
    s32 i = 0;
    if (i < list->modifierCount) {
        struct StatModifier *entry = base;
        struct StatModifier *cursor = base;
        do {
            if (cursor->state.record.kind == state || cursor->state.record.kind == 13)
                value = cursor->callback(entry, value);
            cursor++;
            entry++;
            i++;
        } while (i < list->modifierCount);
    }
    return (s32)value;
}

void StatModifier_InitializeCallbacks(struct ScenePoolActor *p, struct StatModifierOwner *owner)
{
    struct SceneActorStyle *sources = (void *)&p->bodyAnimation;
    s32 i;
    struct StatModifier *slots = p->modifiers;
    {
        u32 zero = 0;
        struct StatModifierState *record = &p->modifiers[0].state;
        struct StatModifier *slot = slots;
        s32 remaining = 23;
        do {
            slot->callback = (void *)zero;
            record->enabled = zero;
            record->owner = owner;
            Memory_Clear(&record->record, sizeof(record->record));
            record = (struct StatModifierState *)((u8 *)record + sizeof(*slot));
            slot++;
        } while (--remaining >= 0);
    }
    for (i = 0; i < sources->count; i++) {
        ScriptRecord_FindModifierRecord(&slots[i].state.record, (s8)sources->entries[i]);
        switch (slots[i].state.record.kind) {
            case 1:
                slots[i].callback = StatModifier_ScaleForGlobalMode;
                slots[i].state.enabled = 1;
                break;
            case 2:
                slots[i].callback = StatModifier_AddForGlobalMode;
                slots[i].state.enabled = 1;
                break;
            case 7:
                slots[i].callback = Modifier_ScaleKindSevenStat;
                slots[i].state.enabled = 1;
                break;
            case 3:
                slots[i].callback = Modifier_AddKindThreeStat;
                slots[i].state.enabled = 1;
                break;
            case 4:
                slots[i].callback = Modifier_ScaleKindFourStat;
                slots[i].state.enabled = 1;
                break;
            case 5:
                slots[i].callback = StatModifier_AddForOwnerMode;
                slots[i].state.enabled = 1;
                break;
            case 6:
                slots[i].callback = Modifier_AddKindSixStat;
                slots[i].state.enabled = 1;
                break;
            case 8:
            case 9:
            case 10:
                slots[i].callback = Modifier_ScaleKindEightToTenStat;
                slots[i].state.enabled = 1;
                break;
            case 13:
                slots[i].callback = StatModifier_ScaleAtOwnerThreshold;
                slots[i].state.enabled = 1;
                break;
            case 14:
                slots[i].callback = StatModifier_ScaleAtOwnerThreshold;
                slots[i].state.enabled = 1;
                break;
            case 0:
            case 11:
            case 12:
            case 15:
                slots[i].callback = Modifier_ScaleDefaultStat;
                slots[i].state.enabled = 1;
                break;
            default:
                continue;
        }
    }
}

s32 CallbackList_RunStateEight(struct ScenePoolActor *list)
{
    s32 value = list->drain.valueBase + (list->drainQuarter >> 2);
    value = CallbackList_RunMatchingState(list, value, 8);
    return value;
}

u32 Modifier_UpdateIdleDrain(void *arg)
{
    struct ScenePoolActor *actor = arg;
    actor->drain.active = 0;
    actor->forwardBias += -0x140;
    return 0;
}

s32 ActorDrain_Update(struct ScenePoolActor *actor)
{
    struct ActorDrainState *state = &actor->drain;
    state->active = 1;
    if (state->entered) {
        if (state->resource)
            state->effect = SpriteSlots_AttachSource(state->resource);
        state->timer = state->duration;
        if (LinkParticipant_IsLocallySelected(actor->linkedPlayer))
            Sound_PlaySong(0x8C);
        state->entered = 0;
    }
    state->timer--;
    if (Mod(state->timer, 4) == 0) {
        s32 amount = CallbackList_RunStateEight(actor);
        s32 health = (s16)actor->timerA - amount;
        if (health < 0)
            health = 0;
        actor->timerA = health;
        actor->forwardBias += actor->drainIncrement;
    }
    if (state->timer < 0 || (s16)actor->timerA <= 0) {
        if (state->effect >= 0)
            TimedSpriteEffect_RequestStop(state->effect);
        state->effect = -1;
        state->callback = Modifier_UpdateIdleDrain;
        state->entered = 1;
    }
    return 1;
}

void Modifier_InitDrainState(struct ActorDrainState *state, void *resource, u16 duration,
                             u8 valueBase)
{
    u8 zero = 0;
    state->active = zero;
    state->timer = zero;
    state->duration = duration;
    state->valueBase = valueBase;
    state->resource = resource;
    state->callback = Modifier_UpdateIdleDrain;
    state->entered = 1;
}

void Modifier_StartDrainIfAboveThreshold(struct ScenePoolActor *o)
{
    if (o->drain.active == 0 && (s16)o->timerA > 200) {
        struct ActorDrainState *p = &o->drain;
        p->callback = (u32(*)(void *))ActorDrain_Update;
        p->entered = 1;
    }
}

void CallbackList_ResetDispatch(struct ScenePoolActor *list)
{
    if (list->drain.effect >= 0)
        TimedSpriteEffect_RequestStop(list->drain.effect);
    list->drain.effect = -1;
    {
        struct ActorDrainState *control = &list->drain;
        control->callback = Modifier_UpdateIdleDrain;
        control->entered = 1;
    }
}
