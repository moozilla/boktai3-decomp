#include "script_options.h"

s32 StateFlags_TestIndexedFlag(s32 index)
{
    if ((u32)index <= 11) {
        u32 mask = 0x800;
        if (((gEngineFlags | gScenePauseFlags) & mask) && index > 5)
            goto enabled;
        if ((gGameState->counters628[3] >> index) & 1)
            goto enabled;
    }
    return 0;
enabled:
    return 1;
}

u32 GameState_SetChallengeCompletionFlag(u32 index)
{
    u32 result;
    if (index <= 11) {
        gGameState->counters628[3] |= 1 << index;
        result = 1;
    } else
        result = 0;
    return result;
}

void GameState_SetChallengeCompletionFlagFromScript(void)
{
    s32 index = Script_GetKeywordValue('c', -1);
    if (index >= 0)
        GameState_SetChallengeCompletionFlag(index);
}

void GameState_ClearChallengeCompletionFlags(void)
{
    gGameState->counters628[3] = 0;
}

s32 MotionTween_Advance(struct ScenePoolActor *object, struct MotionTween *tween)
{
    s32 changed = 0;
    if (tween->valueDelay) {
        tween->valueDelay--;
    } else if (tween->valueFrames) {
        object->steering += Div(tween->targetValue - object->steering, tween->valueFrames);
        if (--tween->valueFrames == 0)
            object->steering = tween->targetValue;
        changed = 1;
    }
    if (tween->coordinateDelay) {
        tween->coordinateDelay--;
    } else if (tween->coordinateFrames) {
        object->lateralSpeed +=
            Div(tween->targetCoordinate - (object->position.z >> 12), tween->coordinateFrames) << 8;
        if (--tween->coordinateFrames == 0)
            object->motionDisplacement += (tween->targetCoordinate - (object->position.z >> 12))
                                          << 8;
        changed = 1;
    }
    object->forwardSpeed = 0x3000;
    return changed;
}

void ClockDisplay_DrawBaseTime(s32 layer)
{
    s32 hour = SunClock_GetBaseHour();
    s32 minute = SunClock_GetBaseMinute();
    s32 value = gGameState->clockDisplayValue;
    Text_DrawNumber(layer, 24, 0, hour, 17, 2, 0);
    Text_DrawNumber(layer, 27, 0, minute, 17, 2, 0);
    Text_DrawNumber(layer, 24, 1, value, 17, 4, 0);
}

void ModifierMenu_DrawClockAndValue(void)
{
    ClockDisplay_DrawBaseTime(2);
}

void Video_DrawClockAndValueOnBackground(s32 layer)
{
    ClockDisplay_DrawBaseTime(layer);
}

void StateFlags_SetCategoryBit(u32 category, u32 bit)
{
    u32 mask = 1 << bit;
    switch (category) {
        case 0:
            gGameState->unk7B8[0] |= mask;
            break;
        case 1:
            gGameState->unk7B8[2] |= mask;
            break;
        case 2:
            gGameState->unk7B8[1] |= mask;
            break;
        case 3:
            gGameState->unk7B8[3] |= mask;
            break;
        case 4:
            gGameState->unk81C |= mask;
            break;
        case 5:
            gGameState->unk7F8 |= mask;
            break;
    }
}

void GameFlag_SetFromScript(void)
{
    s32 missing = -1;
    s32 category = Script_GetKeywordValue(0x6b, missing);
    s32 bit = Script_GetKeywordValue(0x69, missing);
    if (category >= 0 && bit >= 0)
        StateFlags_SetCategoryBit(category, bit);
}

u32 StateFlags_GetCategoryBit(u32 category, u32 bit)
{
    u32 mask = 1 << bit;
    switch (category) {
        case 0:
            return gGameState->unk7B8[0] & mask;
        case 1:
            return gGameState->unk7B8[2] & mask;
        case 2:
            return gGameState->unk7B8[1] & mask;
        case 3:
            return gGameState->unk7B8[3] & mask;
        case 4:
            return gGameState->unk81C & mask;
        case 5:
            return gGameState->unk7F8 & mask;
    }
    return 0;
}

s32 GameFlag_FindFirstSetIndex(u32 category, s32 bitCount)
{
    s32 bit;
    for (bit = 0; bit < bitCount; bit++) {
        if (StateFlags_GetCategoryBit(category, bit))
            return bit;
    }
    return 0;
}

void GameFlag_TestFromScript(void)
{
    struct ScriptUtilityReference result;
    s32 kind, bit;
    Script_ReadReference(&result);
    kind = Script_GetKeywordValue('k', -1);
    bit = Script_GetKeywordValue('i', -1);
    if (kind < 0 || bit < 0)
        Script_WriteReferenceValue(&result, 0, 0);
    else if (StateFlags_GetCategoryBit(kind, bit))
        Script_WriteReferenceValue(&result, 0, 1);
    else
        Script_WriteReferenceValue(&result, 0, 0);
}

struct ScriptOptions *Modifier_GetTableTask(void)
{
    if (!gScriptRecordTable)
        return 0;
    return (void *)gScriptRecordTable;
}

void Modifier_OnTableTaskUpdate(void)
{
}

u32 Modifier_OnTableTaskDestroy(void)
{
    gScriptRecordTable = 0;
}

void ScriptOptions_LoadEntries(struct ScriptOptions *options)
{
    s32 kind;
    if (Script_SeekToKeyword('o')) {
        options->entryScript = (u8 *)Script_GetValueSafe();
        Memory_Clear(options->entries, sizeof(options->entries));
        Script_SetPc(options->entryScript);
        options->entryCount = 0;
        while ((kind = Script_GetValue()) >= 0 && options->entryCount <= 23) {
            options->entries[options->entryCount].kind = kind;
            options->entries[options->entryCount].value = Script_GetValue();
            options->entries[options->entryCount].parameter = Script_GetValue();
            options->entries[options->entryCount].mode = Script_GetValue();
            options->entryCount++;
        }
    }
    if (Script_SeekToKeyword('O'))
        options->option = Script_GetValueSafe();
}

void ScriptOptions_LoadTargets(struct ScriptOptions *options)
{
    s32 kind;
    if (Script_SeekToKeyword('t')) {
        options->targetScript = (u8 *)Script_GetValueSafe();
        Memory_Clear(options->targets, sizeof(options->targets));
        Script_SetPc(options->targetScript);
        options->targetCount = 0;
        while ((kind = Script_GetValue()) >= 0 && options->targetCount <= 5) {
            options->targets[options->targetCount].kind = kind;
            options->targets[options->targetCount].parameters[0] = Script_GetValue();
            options->targets[options->targetCount].parameters[1] = Script_GetValue();
            options->targets[options->targetCount].parameters[2] = Script_GetValue();
            options->targets[options->targetCount].value = Script_GetValue();
            options->targetCount++;
        }
    }
    if (Script_SeekToKeyword('T'))
        options->targetOption = Script_GetValueSafe();
}

void ScriptOptions_LoadWideRecords(struct ScriptOptions *options)
{
    s32 kind;
    if (Script_SeekToKeyword('w')) {
        options->wideScript = (u8 *)Script_GetValueSafe();
        Memory_Clear(options->wideEntries, sizeof(options->wideEntries));
        Script_SetPc(options->wideScript);
        options->wideCount = 0;
        while ((kind = Script_GetValue()) >= 0 && options->wideCount <= 5) {
            options->wideEntries[options->wideCount].kind = kind;
            options->wideEntries[options->wideCount].value = Script_GetValue();
            options->wideEntries[options->wideCount].parameters[0] = Script_GetValue();
            options->wideEntries[options->wideCount].parameters[1] = Script_GetValue();
            options->wideEntries[options->wideCount].parameters[2] = Script_GetValue();
            options->wideEntries[options->wideCount].parameters[3] = Script_GetValue();
            options->wideEntries[options->wideCount].parameterC = Script_GetValue();
            options->wideEntries[options->wideCount].counter = 0;
            options->wideCount++;
        }
    }
    if (Script_SeekToKeyword('W'))
        options->wideOption = Script_GetValueSafe();
}

void ScriptOptions_LoadShortRecords(struct ScriptOptions *options)
{
    s32 kind;
    if (Script_SeekToKeyword('s')) {
        options->shortScript = (u8 *)Script_GetValueSafe();
        Memory_Clear(options->shortEntries, sizeof(options->shortEntries));
        Script_SetPc(options->shortScript);
        options->shortCount = 0;
        while ((kind = Script_GetValue()) >= 0 && options->shortCount <= 5) {
            options->shortEntries[options->shortCount].kind = kind;
            options->shortEntries[options->shortCount].value = Script_GetValue();
            options->shortEntries[options->shortCount].parameters[0] = Script_GetValue();
            options->shortEntries[options->shortCount].parameters[1] = Script_GetValue();
            options->shortEntries[options->shortCount].parameters[2] = Script_GetValue();
            options->shortEntries[options->shortCount].parameters[3] = Script_GetValue();
            options->shortCount++;
        }
    }
    if (Script_SeekToKeyword('S'))
        options->shortOption = Script_GetValueSafe();
}

void ScriptRecord_ReadEntries(struct ScriptOptions *options)
{
    s32 kind;
    if (Script_SeekToKeyword('b')) {
        options->byteScript = (u8 *)Script_GetValueSafe();
        Memory_Clear(options->byteEntries, sizeof(options->byteEntries));
        Script_SetPc(options->byteScript);
        options->byteCount = 0;
        while ((kind = Script_GetValue()) >= 0 && options->byteCount <= 5) {
            options->byteEntries[options->byteCount].kind = kind;
            options->byteEntries[options->byteCount].category = Script_GetValue();
            options->byteEntries[options->byteCount].parameterA = Script_GetValue();
            options->byteEntries[options->byteCount].parameterB = Script_GetValue();
            options->byteEntries[options->byteCount].value = Script_GetValue();
            options->byteCount++;
        }
    }
    if (Script_SeekToKeyword('B'))
        options->byteOption = Script_GetValueSafe();
}

void Modifier_LoadTablesFromScript(struct ScriptOptions *p)
{
    ScriptOptions_LoadEntries(p);
    ScriptOptions_LoadTargets(p);
    ScriptOptions_LoadWideRecords(p);
    ScriptOptions_LoadShortRecords(p);
    ScriptRecord_ReadEntries(p);
}

s32 Modifier_InitTableTask(struct ScriptOptions *options)
{
    Modifier_LoadTablesFromScript(options);
    gScriptRecordTable = (void *)options;
    return 1;
}

struct ScriptOptions *Modifier_CreateTableTask(void)
{
    struct ScriptOptions *options;
    if (gScriptRecordTable != 0)
        return (void *)gScriptRecordTable;
    options = Task_Create(8, sizeof(*options));
    if (options != 0) {
        Task_SetCallbacks(options, (void (*)(void *))Modifier_OnTableTaskUpdate,
                          (void (*)(void *))Modifier_OnTableTaskDestroy);
        if (Modifier_InitTableTask(options) < 0) {
            Task_Kill(options);
            return 0;
        }
    }
    return options;
}
