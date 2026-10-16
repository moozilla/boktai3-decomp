// MATCHING_CONCESSION: register-pinning
#include "scene_actor_script.h"
static inline s32 ReadOrMinusOne(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return -1;
}
static inline u32 ReadKeywordWord(s32 keyword)
{
    u32 value = (u16)Script_GetKeywordValue(keyword, 0);
    return value;
}
static inline u8 IsObjectType(u32 type, u32 expected)
{
    if (type == expected)
        return 1;
    return 0;
}
void SceneActorScript_SetPropertiesFromScript(void)
{
    u32 id = Script_GetKeywordValue('N', 0);
    struct SceneActorScriptPropertiesView *actor;
    s32 value, i;
    if (!id)
        return;
    actor = (void *)SceneActorRegistry_FindById(id);
    if (!actor)
        return;
    if (Script_SeekToKeyword('e')) {
        value = ReadOrMinusOne();
        if (value >= 0) {
            u32 square;
            register u32 *firstSquare asm("r2");
            actor->range = value;
            actor->rangeCopy = value;
            firstSquare = &actor->rangeSquared;
            square = (u32)actor->range * actor->range;
            *firstSquare = square;
            actor->rangeSquaredCopy = square;
        }
        value = ReadOrMinusOne();
        if (value >= 0)
            actor->behaviorEA = value;
    }
    if (Script_SeekToKeyword('a')) {
        for (i = 0; i < 4; i++) {
            value = ReadOrMinusOne();
            if (value >= 0)
                actor->parametersB4[i] = value;
            value = ReadOrMinusOne();
            if (value >= 0)
                actor->parametersBC[i] = value;
            value = ReadOrMinusOne();
            if (value >= 0)
                actor->parametersCC[i] = value;
            value = ReadOrMinusOne();
            if (value >= 0)
                actor->parametersC4[i] = value;
        }
    }
    if (Script_SeekToKeyword('A')) {
        value = ReadOrMinusOne();
        if (value >= 0) {
            s32 health;
            actor->baseHealth = value;
            if (IsObjectType(actor->objectType, 1))
                actor->quarterHealth = (u16)actor->baseHealth >> 2;
            health = actor->baseHealth;
            if (health <= 0)
                health = 0;
            actor->health = health;
        }
        value = ReadOrMinusOne();
        if (value >= 0)
            actor->property9E = value;
        value = ReadOrMinusOne();
        if (value >= 0)
            actor->property9F = value;
        value = ReadOrMinusOne();
        if (value >= 0)
            actor->propertyA0 = value;
        value = ReadOrMinusOne();
        if (value >= 0)
            actor->propertyA2 = value;
        value = ReadOrMinusOne();
        if (value >= 0)
            actor->propertyA4 = value;
    }
    if (Script_SeekToKeyword('l')) {
        value = ReadOrMinusOne();
        if (value >= 0)
            actor->propertyA6 = value;
        value = ReadOrMinusOne();
        if (value >= 0)
            actor->propertyA8 = value;
        value = ReadOrMinusOne();
        if (value >= 0 && IsObjectType(actor->objectType, 1))
            actor->control.property42 = value;
    }
    if (Script_SeekToKeyword('n')) {
        struct SceneActorScriptControlView *control = &actor->control;
        value = ReadOrMinusOne();
        if (value >= 0) {
            u32 square;
            control->radius = value;
            square = (u32)control->radius * control->radius;
            control->radiusSquared = square;
        }
        value = ReadOrMinusOne();
        if (value >= 0)
            control->parameter4C = value;
    }
    if (Script_SeekToKeyword('R'))
        actor->property1EC = ReadKeywordWord('R');
    if (Script_SeekToKeyword('F'))
        actor->property1F0 = ReadKeywordWord('F');
    if (Script_SeekToKeyword('I'))
        actor->property1F4 = ReadKeywordWord('I');
    if (Script_SeekToKeyword('D'))
        actor->property1F8 = ReadKeywordWord('D');
    if (Script_SeekToKeyword('G')) {
        for (i = 0; i < 5; i++) {
            value = ReadOrMinusOne();
            if (value >= 0)
                actor->parameters1FC[i] = value;
        }
    }
    if (Script_SeekToKeyword('K'))
        actor->propertyB2 = Script_GetKeywordValue('K', 2);
}
